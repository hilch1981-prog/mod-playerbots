#ifndef CHIPA_PLAYERBOT_PLAYER_UPDATE_ADAPTER_H
#define CHIPA_PLAYERBOT_PLAYER_UPDATE_ADAPTER_H

#include <cstdint>

class Player;

namespace chipa
{
namespace playerbots
{
    typedef bool (*IsManagedPlayerCallback)(Player*);
    typedef void (*PlayerAiUpdateCallback)(Player*, std::uint32_t);
    typedef void (*PlayerMgrUpdateCallback)(Player*, std::uint32_t);

    struct PlayerUpdateBackend
    {
        IsManagedPlayerCallback isManagedPlayer = nullptr;
        PlayerAiUpdateCallback updateAI = nullptr;
        PlayerMgrUpdateCallback updateManager = nullptr;
    };

    // Backend callbacks are installed by the PlayerBot-specific compatibility
    // layer after its manager/AI code is available. Until then, all player
    // updates return immediately and human behavior remains untouched.
    void ConfigurePlayerUpdateBackend(PlayerUpdateBackend const& backend);
    void ResetPlayerUpdateBackend();

    // Registers the module-owned callback behind PlayerUpdateBridge. This does
    // not make G2 PASS by itself; runtime/build/game evidence is still required.
    void RegisterPlayerUpdateAdapter();
    void UnregisterPlayerUpdateAdapter();
}
}

#endif
