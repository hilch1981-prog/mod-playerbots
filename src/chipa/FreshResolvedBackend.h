#ifndef CHIPA_PLAYERBOT_FRESH_RESOLVED_BACKEND_H
#define CHIPA_PLAYERBOT_FRESH_RESOLVED_BACKEND_H

#include "PlayerUpdateAdapter.h"

#include <cstdint>

namespace chipa
{
namespace playerbots
{
    // Compile-time binding from donor-owned AI/manager accessors to the generic
    // PlayerUpdateBackend. A donor-specific TU instantiates this template with
    // PlayerbotsMgr accessors and public UpdateAI wrappers; no donor headers are
    // required by the generic Chipa bridge itself.
    //
    // IsManagedPlayer may resolve objects for classification, but each work
    // thunk resolves again immediately before update. This deliberately favors
    // lifetime safety over pointer reuse across a lifecycle/SelfBot transition.
    template <
        typename AiType,
        typename ManagerType,
        AiType* (*ResolveAI)(Player*),
        void (*ApplyAIUpdate)(AiType*, std::uint32_t),
        ManagerType* (*ResolveManager)(Player*),
        void (*ApplyManagerUpdate)(ManagerType*, std::uint32_t)>
    struct FreshResolvedBackend
    {
        static bool IsManagedPlayer(Player* player)
        {
            if (!player)
                return false;

            // These function pointers are compile-time template arguments and
            // therefore cannot be null. Avoid redundant pointer tests here:
            // with -Waddress -Werror GCC correctly diagnoses them as always
            // true. The resolved donor object itself remains nullable.
            if (ResolveAI(player))
                return true;

            return ResolveManager(player) != nullptr;
        }

        static void UpdateAI(Player* player, std::uint32_t diff)
        {
            if (!player)
                return;

            AiType* const ai = ResolveAI(player);
            if (ai)
                ApplyAIUpdate(ai, diff);
        }

        static void UpdateManager(Player* player, std::uint32_t diff)
        {
            if (!player)
                return;

            ManagerType* const manager = ResolveManager(player);
            if (manager)
                ApplyManagerUpdate(manager, diff);
        }

        static PlayerUpdateBackend MakeBackend()
        {
            PlayerUpdateBackend backend;
            backend.isManagedPlayer = &IsManagedPlayer;
            backend.updateAI = &UpdateAI;
            backend.updateManager = &UpdateManager;
            return backend;
        }
    };
}
}

#endif
