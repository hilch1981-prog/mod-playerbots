#include "MonkTriggers.h"

#include "Player.h"

// MoP 5.4.8 build 18414 aura IDs verified against the SkyFire 5.4.8
// spell_monk implementation. These are aura-state checks only; spell casts
// still resolve through the Legends engine by spell name.
namespace
{
    constexpr uint32 MONK_MODERATE_STAGGER = 124274;
    constexpr uint32 MONK_HEAVY_STAGGER = 124273;
}

bool ModerateStaggerTrigger::IsActive()
{
    return bot && bot->HasAura(MONK_MODERATE_STAGGER);
}

bool HeavyStaggerTrigger::IsActive()
{
    return bot && bot->HasAura(MONK_HEAVY_STAGGER);
}

bool DetoxMagicTrigger::IsActive()
{
    return bot && bot->GetSpecialization() == Specializations::SPEC_MONK_MISTWEAVER &&
        NeedCureTrigger::IsActive();
}

bool DetoxMagicOnPartyTrigger::IsActive()
{
    return bot && bot->GetSpecialization() == Specializations::SPEC_MONK_MISTWEAVER &&
        PartyMemberNeedCureTrigger::IsActive();
}
