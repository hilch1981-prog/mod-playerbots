#include "PlayerUpdateAdapter.h"
#include "PlayerUpdateBridge.h"

#include <atomic>

namespace
{
    std::atomic<chipa::playerbots::IsManagedPlayerCallback> g_isManagedPlayer(nullptr);
    std::atomic<chipa::playerbots::PlayerAiUpdateCallback> g_updateAI(nullptr);
    std::atomic<chipa::playerbots::PlayerMgrUpdateCallback> g_updateManager(nullptr);
    std::atomic<std::uint64_t> g_backendGeneration(0);

    struct BackendSnapshot
    {
        chipa::playerbots::IsManagedPlayerCallback isManagedPlayer;
        chipa::playerbots::PlayerAiUpdateCallback updateAI;
        chipa::playerbots::PlayerMgrUpdateCallback updateManager;
    };

    BackendSnapshot LoadBackendSnapshot()
    {
        for (;;)
        {
            std::uint64_t const generationBefore =
                g_backendGeneration.load(std::memory_order_acquire);

            // Odd generations mean a writer is replacing/resetting the
            // backend. Wait for a complete publication rather than combining
            // callbacks from two different backend generations.
            if (generationBefore & 1U)
                continue;

            BackendSnapshot snapshot = {
                g_isManagedPlayer.load(std::memory_order_acquire),
                g_updateAI.load(std::memory_order_acquire),
                g_updateManager.load(std::memory_order_acquire)};

            std::uint64_t const generationAfter =
                g_backendGeneration.load(std::memory_order_acquire);

            if (generationBefore == generationAfter)
                return snapshot;
        }
    }

    void BeginBackendWrite()
    {
        g_backendGeneration.fetch_add(1, std::memory_order_acq_rel);
    }

    void EndBackendWrite()
    {
        g_backendGeneration.fetch_add(1, std::memory_order_release);
    }

    void HandlePlayerUpdate(Player* player, std::uint32_t diff)
    {
        if (!player)
            return;

        BackendSnapshot const backend = LoadBackendSnapshot();

        // G2 invariant: an ordinary human player must leave the PlayerBot path
        // before any AI or manager callback can run.
        if (!backend.isManagedPlayer || !backend.isManagedPlayer(player))
            return;

        if (backend.updateAI)
            backend.updateAI(player, diff);

        if (backend.updateManager)
            backend.updateManager(player, diff);
    }
}

namespace chipa
{
namespace playerbots
{
    void ConfigurePlayerUpdateBackend(PlayerUpdateBackend const& backend)
    {
        // Publish the callback trio as one coherent generation. Runtime backend
        // replacement is uncommon, but keeping reconfiguration coherent avoids
        // pairing a predicate from one backend with work callbacks from another.
        BeginBackendWrite();
        g_isManagedPlayer.store(backend.isManagedPlayer, std::memory_order_relaxed);
        g_updateAI.store(backend.updateAI, std::memory_order_relaxed);
        g_updateManager.store(backend.updateManager, std::memory_order_relaxed);
        EndBackendWrite();
    }

    void ResetPlayerUpdateBackend()
    {
        BeginBackendWrite();
        g_isManagedPlayer.store(nullptr, std::memory_order_relaxed);
        g_updateAI.store(nullptr, std::memory_order_relaxed);
        g_updateManager.store(nullptr, std::memory_order_relaxed);
        EndBackendWrite();
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
