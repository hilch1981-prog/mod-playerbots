#include "PlayerUpdateAdapter.h"
#include "PlayerUpdateBridge.h"

#include <atomic>

namespace
{
    std::atomic<chipa::playerbots::IsManagedPlayerCallback> g_isManagedPlayer(nullptr);
    std::atomic<chipa::playerbots::PlayerAiUpdateCallback> g_updateAI(nullptr);
    std::atomic<chipa::playerbots::PlayerMgrUpdateCallback> g_updateManager(nullptr);

    void HandlePlayerUpdate(Player* player, std::uint32_t diff)
    {
        chipa::playerbots::IsManagedPlayerCallback isManagedPlayer =
            g_isManagedPlayer.load(std::memory_order_acquire);

        // G2 invariant: an ordinary human player must leave the PlayerBot path
        // before any AI or manager callback can run.
        if (!isManagedPlayer || !isManagedPlayer(player))
            return;

        if (chipa::playerbots::PlayerAiUpdateCallback updateAI =
                g_updateAI.load(std::memory_order_acquire))
        {
            updateAI(player, diff);
        }

        if (chipa::playerbots::PlayerMgrUpdateCallback updateManager =
                g_updateManager.load(std::memory_order_acquire))
        {
            updateManager(player, diff);
        }
    }
}

namespace chipa
{
namespace playerbots
{
    void ConfigurePlayerUpdateBackend(PlayerUpdateBackend const& backend)
    {
        // Publish work callbacks first and the predicate last. A concurrent
        // update can therefore only enter the backend after all callbacks from
        // the same configuration are visible.
        g_updateAI.store(backend.updateAI, std::memory_order_release);
        g_updateManager.store(backend.updateManager, std::memory_order_release);
        g_isManagedPlayer.store(backend.isManagedPlayer, std::memory_order_release);
    }

    void ResetPlayerUpdateBackend()
    {
        // Stop new entries first, then clear the work callbacks.
        g_isManagedPlayer.store(nullptr, std::memory_order_release);
        g_updateAI.store(nullptr, std::memory_order_release);
        g_updateManager.store(nullptr, std::memory_order_release);
    }

    void RegisterPlayerUpdateAdapter()
    {
        SetPlayerUpdateCallback(&HandlePlayerUpdate);
    }

    void UnregisterPlayerUpdateAdapter()
    {
        SetPlayerUpdateCallback(nullptr);
        ResetPlayerUpdateBackend();
    }
}
}
