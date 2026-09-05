#ifndef CHIPA_PLAYERBOT_PLAYER_LIFECYCLE_BRIDGE_H
#define CHIPA_PLAYERBOT_PLAYER_LIFECYCLE_BRIDGE_H

class Player;

namespace chipa
{
namespace playerbots
{
    typedef void (*PlayerLifecycleCallback)(Player*);

    // Generic lifecycle seams used by the Chipa PlayerScript integration.
    // Concrete PlayerBot ownership/cleanup remains in a later compatibility
    // backend; with no callback installed these dispatchers are no-ops.
    void SetPlayerLoginCallback(PlayerLifecycleCallback callback);
    void SetPlayerLogoutCallback(PlayerLifecycleCallback callback);
    void DispatchPlayerLogin(Player* player);
    void DispatchPlayerLogout(Player* player);
}
}

#endif
