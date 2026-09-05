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
    Player* g_managed = nullptr;
    std::vector<int> g_events;
    std::uint32_t g_lastDiff = 0;
    int g_loginCount = 0;
    int g_logoutCount = 0;
    thread_local int g_stressPair = 0;

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
