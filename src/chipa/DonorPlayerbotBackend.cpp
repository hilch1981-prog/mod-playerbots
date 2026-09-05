/*
 * Chipa PlayerBot V2 - MoP 5.4.8 Build 18414
 *
 * Donor-specific G2 compatibility backend.
 *
 * This translation unit is deliberately isolated from the generic Chipa
 * bridge. It is not added to chipa_module.cmake until the donor PlayerBot
 * source closure required by PlayerbotAI/PlayerbotMgr is adapted and links
 * against the Chipa MoP runtime. Keeping that activation separate lets us
 * review the concrete ownership/update wiring without pretending G2 is ready.
 */

#include "FreshResolvedBackend.h"

#include "../Bot/PlayerbotAI.h"
#include "../Bot/PlayerbotMgr.h"

namespace chipa
{
namespace playerbots
{
namespace
{
    PlayerbotAI* ResolvePlayerbotAI(Player* player)
    {
        return PlayerbotsMgr::instance().GetPlayerbotAI(player);
    }

    void ApplyPlayerbotAIUpdate(PlayerbotAI* ai, std::uint32_t diff)
    {
        if (ai)
            ai->UpdateAI(diff);
    }

    PlayerbotMgr* ResolvePlayerbotManager(Player* player)
    {
        return PlayerbotsMgr::instance().GetPlayerbotMgr(player);
    }

    void ApplyPlayerbotManagerUpdate(PlayerbotMgr* manager, std::uint32_t diff)
    {
        if (manager)
            manager->UpdateAI(diff);
    }

    typedef FreshResolvedBackend<
        PlayerbotAI,
        PlayerbotMgr,
        &ResolvePlayerbotAI,
        &ApplyPlayerbotAIUpdate,
        &ResolvePlayerbotManager,
        &ApplyPlayerbotManagerUpdate>
        DonorPlayerUpdateBackend;
}

void ConfigureDonorPlayerUpdateBackend()
{
    ConfigurePlayerUpdateBackend(DonorPlayerUpdateBackend::MakeBackend());
}
}
}
