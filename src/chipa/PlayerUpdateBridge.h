#ifndef CHIPA_PLAYERBOT_PLAYER_UPDATE_BRIDGE_H
#define CHIPA_PLAYERBOT_PLAYER_UPDATE_BRIDGE_H

#include <cstdint>

class Player;

namespace chipa
{
namespace playerbots
{
    typedef void (*PlayerUpdateCallback)(Player*, std::uint32_t);

    // Compatibility-adapter seam. The generic PlayerScript never includes
    // PlayerBot manager/AI headers and therefore cannot own PlayerBot policy.
    void SetPlayerUpdateCallback(PlayerUpdateCallback callback);
    void DispatchPlayerUpdate(Player* player, std::uint32_t diff);
}
}

#endif
