#include "PlayerLifecycleBridge.h"

#include <atomic>

namespace
{
    std::atomic<chipa::playerbots::PlayerLifecycleCallback> g_loginCallback(nullptr);
    std::atomic<chipa::playerbots::PlayerLifecycleCallback> g_logoutCallback(nullptr);
}

namespace chipa
{
namespace playerbots
{
    void SetPlayerLoginCallback(PlayerLifecycleCallback callback)
    {
        g_loginCallback.store(callback, std::memory_order_release);
    }

    void SetPlayerLogoutCallback(PlayerLifecycleCallback callback)
    {
        g_logoutCallback.store(callback, std::memory_order_release);
    }

    void DispatchPlayerLogin(Player* player)
    {
        if (!player)
            return;

        PlayerLifecycleCallback callback = g_loginCallback.load(std::memory_order_acquire);
        if (callback)
            callback(player);
    }

    void DispatchPlayerLogout(Player* player)
    {
        if (!player)
            return;

        PlayerLifecycleCallback callback = g_logoutCallback.load(std::memory_order_acquire);
        if (callback)
            callback(player);
    }
}
}
