#include "FreshResolvedBackend.h"
#include "FreshResolvedUpdate.h"
#include "PlayerLifecycleBridge.h"
#include "PlayerUpdateAdapter.h"
#include "PlayerUpdateBridge.h"

#include <atomic>
#include <cassert>
#include <cstdint>
#include <thread>
#include <vector>

class Player
{
public:
    explicit Player(int value) : id(value) { }
    int id;
};

namespace
{
    struct FakeAI
    {
        explicit FakeAI(int value) : id(value) { }
        int id;
    };

    struct FakeManager
    {
        explicit FakeManager(int value) : id(value) { }
        int id;
    };

    Player* g_managed = nullptr;
    std::vector<int> g_events;
    std::uint32_t g_lastDiff = 0;
    int g_loginCount = 0;
    int g_logoutCount = 0;
    thread_local int g_stressPair = 0;

    FakeAI* g_resolvedAI = nullptr;
    FakeManager* g_resolvedManager = nullptr;
    int g_aiResolveCount = 0;
    int g_managerResolveCount = 0;
    int g_lastResolvedAI = 0;
    int g_lastResolvedManager = 0;
    std::uint32_t g_lastResolvedAIDiff = 0;
    std::uint32_t g_lastResolvedManagerDiff = 0;

    bool IsManaged(Player* player)
    {
        return player == g_managed;
    }

    void UpdateAI(Player* player, std::uint32_t diff)
    {
        assert(player == g_managed);
        g_lastDiff = diff;
        g_events.push_back(1);
    }

    void UpdateManager(Player* player, std::uint32_t diff)
    {
        assert(player == g_managed);
        assert(diff == g_lastDiff);
        g_events.push_back(2);
    }

    void UpdateAIReplacement(Player* player, std::uint32_t diff)
    {
        assert(player == g_managed);
        g_lastDiff = diff;
        g_events.push_back(3);
    }

    void UpdateManagerReplacement(Player* player, std::uint32_t diff)
    {
        assert(player == g_managed);
        assert(diff == g_lastDiff);
        g_events.push_back(4);
    }

    void StressAUpdateAI(Player* player, std::uint32_t)
    {
        assert(player == g_managed);
        g_stressPair = 1;
    }

    void StressAUpdateManager(Player* player, std::uint32_t)
    {
        assert(player == g_managed);
        assert(g_stressPair == 1);
    }

    void StressBUpdateAI(Player* player, std::uint32_t)
    {
        assert(player == g_managed);
        g_stressPair = 2;
    }

    void StressBUpdateManager(Player* player, std::uint32_t)
    {
        assert(player == g_managed);
        assert(g_stressPair == 2);
    }

    FakeAI* ResolveAI(Player* player)
    {
        ++g_aiResolveCount;
        return player == g_managed ? g_resolvedAI : nullptr;
    }

    FakeManager* ResolveManager(Player* player)
    {
        ++g_managerResolveCount;
        return player == g_managed ? g_resolvedManager : nullptr;
    }

    void UpdateResolvedAI(FakeAI* ai, std::uint32_t diff)
    {
        assert(ai != nullptr);
        g_lastResolvedAI = ai->id;
        g_lastResolvedAIDiff = diff;
        g_events.push_back(5);
    }

    void UpdateResolvedManager(FakeManager* manager, std::uint32_t diff)
    {
        assert(manager != nullptr);
        g_lastResolvedManager = manager->id;
        g_lastResolvedManagerDiff = diff;
        g_events.push_back(6);
    }

    void OnLogin(Player* player)
    {
        assert(player != nullptr);
        ++g_loginCount;
    }

    void OnLogout(Player* player)
    {
        assert(player != nullptr);
        ++g_logoutCount;
    }
}

int main()
{
    Player human(1);
    Player bot(2);
    g_managed = &bot;

    // Registering the adapter without a concrete backend must remain a no-op.
    chipa::playerbots::RegisterPlayerUpdateAdapter();
    chipa::playerbots::DispatchPlayerUpdate(&human, 10);
    chipa::playerbots::DispatchPlayerUpdate(&bot, 10);
    assert(g_events.empty());

    chipa::playerbots::PlayerUpdateBackend backend;
    backend.isManagedPlayer = &IsManaged;
    backend.updateAI = &UpdateAI;
    backend.updateManager = &UpdateManager;
    chipa::playerbots::ConfigurePlayerUpdateBackend(backend);

    // Ordinary humans must return before AI/manager work.
    chipa::playerbots::DispatchPlayerUpdate(&human, 20);
    assert(g_events.empty());

    // Managed players preserve the donor public scheduling order: AI -> mgr.
    chipa::playerbots::DispatchPlayerUpdate(&bot, 33);
    assert(g_events.size() == 2);
    assert(g_events[0] == 1);
    assert(g_events[1] == 2);
    assert(g_lastDiff == 33);

    // Replacing the backend must make the next dispatch use the new callback
    // generation as a coherent set rather than mixing old/new work callbacks.
    g_events.clear();
    chipa::playerbots::PlayerUpdateBackend replacement;
    replacement.isManagedPlayer = &IsManaged;
    replacement.updateAI = &UpdateAIReplacement;
    replacement.updateManager = &UpdateManagerReplacement;
    chipa::playerbots::ConfigurePlayerUpdateBackend(replacement);
    chipa::playerbots::DispatchPlayerUpdate(&bot, 34);
    assert(g_events.size() == 2);
    assert(g_events[0] == 3);
    assert(g_events[1] == 4);
    assert(g_lastDiff == 34);

    // A partially populated backend is valid while compatibility work is
    // staged; absent work callbacks must remain safe no-ops.
    g_events.clear();
    chipa::playerbots::PlayerUpdateBackend aiOnly;
    aiOnly.isManagedPlayer = &IsManaged;
    aiOnly.updateAI = &UpdateAI;
    chipa::playerbots::ConfigurePlayerUpdateBackend(aiOnly);
    chipa::playerbots::DispatchPlayerUpdate(&bot, 35);
    assert(g_events.size() == 1);
    assert(g_events[0] == 1);
    assert(g_lastDiff == 35);

    // Exercise the writer-serialization contract under concurrent backend
    // replacement. The dispatcher must never observe AI from one generation
    // paired with the manager callback from the other generation.
    chipa::playerbots::PlayerUpdateBackend stressA;
    stressA.isManagedPlayer = &IsManaged;
    stressA.updateAI = &StressAUpdateAI;
    stressA.updateManager = &StressAUpdateManager;

    chipa::playerbots::PlayerUpdateBackend stressB;
    stressB.isManagedPlayer = &IsManaged;
    stressB.updateAI = &StressBUpdateAI;
    stressB.updateManager = &StressBUpdateManager;

    std::atomic<bool> startStress(false);
    std::thread writerA([&]() {
        while (!startStress.load(std::memory_order_acquire))
        {
        }
        for (int i = 0; i < 20000; ++i)
            chipa::playerbots::ConfigurePlayerUpdateBackend(stressA);
    });
    std::thread writerB([&]() {
        while (!startStress.load(std::memory_order_acquire))
        {
        }
        for (int i = 0; i < 20000; ++i)
            chipa::playerbots::ConfigurePlayerUpdateBackend(stressB);
    });
    std::thread dispatcher([&]() {
        while (!startStress.load(std::memory_order_acquire))
        {
        }
        for (std::uint32_t i = 0; i < 50000; ++i)
            chipa::playerbots::DispatchPlayerUpdate(&bot, i);
    });

    startStress.store(true, std::memory_order_release);
    writerA.join();
    writerB.join();
    dispatcher.join();

    g_events.clear();
    chipa::playerbots::ResetPlayerUpdateBackend();
    chipa::playerbots::DispatchPlayerUpdate(&bot, 44);
    assert(g_events.empty());

    chipa::playerbots::UnregisterPlayerUpdateAdapter();
    chipa::playerbots::DispatchPlayerUpdate(&bot, 55);
    assert(g_events.empty());

    // The donor compatibility helper must resolve AI and manager fresh on
    // every dispatch, in AI -> manager order, and must not retain stale raw
    // pointers across lifecycle/SelfBot ownership changes.
    FakeAI aiA(101);
    FakeAI aiB(102);
    FakeManager managerA(201);
    FakeManager managerB(202);
    g_resolvedAI = &aiA;
    g_resolvedManager = &managerA;
    g_events.clear();
    g_aiResolveCount = 0;
    g_managerResolveCount = 0;

    chipa::playerbots::DispatchFreshResolvedUpdates(
        &bot, 70, &ResolveAI, &UpdateResolvedAI, &ResolveManager, &UpdateResolvedManager);
    assert(g_events.size() == 2);
    assert(g_events[0] == 5);
    assert(g_events[1] == 6);
    assert(g_aiResolveCount == 1);
    assert(g_managerResolveCount == 1);
    assert(g_lastResolvedAI == 101);
    assert(g_lastResolvedManager == 201);
    assert(g_lastResolvedAIDiff == 70);
    assert(g_lastResolvedManagerDiff == 70);

    // Replace both resolved objects without changing the helper. The next tick
    // must observe the new objects, proving there is no cross-tick pointer cache.
    g_resolvedAI = &aiB;
    g_resolvedManager = &managerB;
    g_events.clear();
    chipa::playerbots::DispatchFreshResolvedUpdates(
        &bot, 71, &ResolveAI, &UpdateResolvedAI, &ResolveManager, &UpdateResolvedManager);
    assert(g_events.size() == 2);
    assert(g_events[0] == 5);
    assert(g_events[1] == 6);
    assert(g_aiResolveCount == 2);
    assert(g_managerResolveCount == 2);
    assert(g_lastResolvedAI == 102);
    assert(g_lastResolvedManager == 202);
    assert(g_lastResolvedAIDiff == 71);
    assert(g_lastResolvedManagerDiff == 71);

    // Match the donor hook's independent null handling: a missing AI does not
    // suppress manager work, and a missing manager does not suppress AI work.
    g_resolvedAI = nullptr;
    g_resolvedManager = &managerA;
    g_events.clear();
    chipa::playerbots::DispatchFreshResolvedUpdates(
        &bot, 72, &ResolveAI, &UpdateResolvedAI, &ResolveManager, &UpdateResolvedManager);
    assert(g_events.size() == 1);
    assert(g_events[0] == 6);
    assert(g_lastResolvedManagerDiff == 72);

    g_resolvedAI = &aiA;
    g_resolvedManager = nullptr;
    g_events.clear();
    chipa::playerbots::DispatchFreshResolvedUpdates(
        &bot, 73, &ResolveAI, &UpdateResolvedAI, &ResolveManager, &UpdateResolvedManager);
    assert(g_events.size() == 1);
    assert(g_events[0] == 5);
    assert(g_lastResolvedAIDiff == 73);

    int const aiResolveBeforeNull = g_aiResolveCount;
    int const managerResolveBeforeNull = g_managerResolveCount;
    g_events.clear();
    chipa::playerbots::DispatchFreshResolvedUpdates<Player, FakeAI, FakeManager>(
        nullptr, 74, &ResolveAI, &UpdateResolvedAI, &ResolveManager, &UpdateResolvedManager);
    assert(g_events.empty());
    assert(g_aiResolveCount == aiResolveBeforeNull);
    assert(g_managerResolveCount == managerResolveBeforeNull);

    // Exercise the typed production binding that a donor-specific TU will
    // instantiate with PlayerbotsMgr accessors and public UpdateAI wrappers.
    typedef chipa::playerbots::FreshResolvedBackend<
        FakeAI,
        FakeManager,
        &ResolveAI,
        &UpdateResolvedAI,
        &ResolveManager,
        &UpdateResolvedManager> BoundBackend;

    chipa::playerbots::RegisterPlayerUpdateAdapter();
    chipa::playerbots::ConfigurePlayerUpdateBackend(BoundBackend::MakeBackend());

    g_resolvedAI = &aiA;
    g_resolvedManager = &managerA;
    g_events.clear();
    chipa::playerbots::DispatchPlayerUpdate(&human, 80);
    assert(g_events.empty());

    g_events.clear();
    chipa::playerbots::DispatchPlayerUpdate(&bot, 81);
    assert(g_events.size() == 2);
    assert(g_events[0] == 5);
    assert(g_events[1] == 6);
    assert(g_lastResolvedAI == 101);
    assert(g_lastResolvedManager == 201);
    assert(g_lastResolvedAIDiff == 81);
    assert(g_lastResolvedManagerDiff == 81);

    // Change ownership targets between dispatches. The bound backend must use
    // the replacement objects immediately rather than any prior raw pointer.
    g_resolvedAI = &aiB;
    g_resolvedManager = &managerB;
    g_events.clear();
    chipa::playerbots::DispatchPlayerUpdate(&bot, 82);
    assert(g_events.size() == 2);
    assert(g_events[0] == 5);
    assert(g_events[1] == 6);
    assert(g_lastResolvedAI == 102);
    assert(g_lastResolvedManager == 202);
    assert(g_lastResolvedAIDiff == 82);
    assert(g_lastResolvedManagerDiff == 82);

    // Classification and work remain null-safe if only one donor-owned object
    // exists, matching the donor hook's independent AI/manager branches.
    g_resolvedAI = nullptr;
    g_resolvedManager = &managerA;
    g_events.clear();
    chipa::playerbots::DispatchPlayerUpdate(&bot, 83);
    assert(g_events.size() == 1);
    assert(g_events[0] == 6);
    assert(g_lastResolvedManagerDiff == 83);

    g_resolvedAI = &aiA;
    g_resolvedManager = nullptr;
    g_events.clear();
    chipa::playerbots::DispatchPlayerUpdate(&bot, 84);
    assert(g_events.size() == 1);
    assert(g_events[0] == 5);
    assert(g_lastResolvedAIDiff == 84);

    g_resolvedAI = nullptr;
    g_resolvedManager = nullptr;
    g_events.clear();
    chipa::playerbots::DispatchPlayerUpdate(&bot, 85);
    assert(g_events.empty());

    chipa::playerbots::UnregisterPlayerUpdateAdapter();

    // The generic bridges reject null Player pointers and are no-ops until a
    // concrete lifecycle backend is installed.
    chipa::playerbots::DispatchPlayerUpdate(nullptr, 66);
    chipa::playerbots::DispatchPlayerLogin(nullptr);
    chipa::playerbots::DispatchPlayerLogout(nullptr);
    chipa::playerbots::DispatchPlayerLogin(&human);
    chipa::playerbots::DispatchPlayerLogout(&human);
    assert(g_loginCount == 0);
    assert(g_logoutCount == 0);

    chipa::playerbots::SetPlayerLoginCallback(&OnLogin);
    chipa::playerbots::SetPlayerLogoutCallback(&OnLogout);
    chipa::playerbots::DispatchPlayerLogin(&human);
    chipa::playerbots::DispatchPlayerLogout(&human);
    assert(g_loginCount == 1);
    assert(g_logoutCount == 1);

    // Clearing the callbacks restores the no-op path used before the concrete
    // SelfBot ownership backend is available.
    chipa::playerbots::SetPlayerLoginCallback(nullptr);
    chipa::playerbots::SetPlayerLogoutCallback(nullptr);
    chipa::playerbots::DispatchPlayerLogin(&bot);
    chipa::playerbots::DispatchPlayerLogout(&bot);
    assert(g_loginCount == 1);
    assert(g_logoutCount == 1);

    return 0;
}
