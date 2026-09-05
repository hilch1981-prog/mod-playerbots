#ifndef CHIPA_PLAYERBOT_FRESH_RESOLVED_UPDATE_H
#define CHIPA_PLAYERBOT_FRESH_RESOLVED_UPDATE_H

#include <cstdint>

namespace chipa
{
namespace playerbots
{
    // Resolve both donor-owned objects from the Player on every dispatch.
    // This intentionally keeps no PlayerbotAI/PlayerbotMgr raw pointer across
    // ticks, login/logout, or SelfBot ownership transitions.
    //
    // The helper is donor-type agnostic so the generic Chipa boundary remains
    // free of PlayerBot/Core implementation headers. A donor-specific
    // compatibility TU supplies the resolvers and public UpdateAI wrappers.
    template <typename PlayerType, typename AiType, typename ManagerType>
    void DispatchFreshResolvedUpdates(
        PlayerType* player,
        std::uint32_t diff,
        AiType* (*resolveAI)(PlayerType*),
        void (*updateAI)(AiType*, std::uint32_t),
        ManagerType* (*resolveManager)(PlayerType*),
        void (*updateManager)(ManagerType*, std::uint32_t))
    {
        if (!player)
            return;

        // Preserve the donor's public scheduling order. AI and manager are
        // resolved independently so a lifecycle change cannot make a cached
        // object survive into a later dispatch.
        if (resolveAI && updateAI)
        {
            AiType* const ai = resolveAI(player);
            if (ai)
                updateAI(ai, diff);
        }

        if (resolveManager && updateManager)
        {
            ManagerType* const manager = resolveManager(player);
            if (manager)
                updateManager(manager, diff);
        }
    }
}
}

#endif
