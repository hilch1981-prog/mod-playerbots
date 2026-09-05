#include "PlayerUpdateAdapter.h"
#include "PlayerUpdateBridge.h"

#include <cassert>
#include <cstdint>
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

    g_events.clear();
    chipa::playerbots::ResetPlayerUpdateBackend();
    chipa::playerbots::DispatchPlayerUpdate(&bot, 44);
    assert(g_events.empty());

    chipa::playerbots::UnregisterPlayerUpdateAdapter();
    chipa::playerbots::DispatchPlayerUpdate(&bot, 55);
    assert(g_events.empty());

    // The generic bridge itself also rejects a null Player pointer.
    chipa::playerbots::DispatchPlayerUpdate(nullptr, 66);
    assert(g_events.empty());

    return 0;
}
