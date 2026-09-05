#include "PlayerUpdateBridge.h"

#include <atomic>

namespace
{
    std::atomic<chipa::playerbots::PlayerUpdateCallback> g_playerUpdateCallback(nullptr);
}

namespace chipa
{
namespace playerbots
{
    void SetPlayerUpdateCallback(PlayerUpdateCallback callback)
    {
        g_playerUpdateCallback.store(callback, std::memory_order_release);
    }

    void DispatchPlayerUpdate(Player* player, std::uint32_t diff)
    {
        if (!player)
            return;

        PlayerUpdateCallback callback = g_playerUpdateCallback.load(std::memory_order_acquire);
        if (callback)
            callback(player, diff);
    }
}
}
