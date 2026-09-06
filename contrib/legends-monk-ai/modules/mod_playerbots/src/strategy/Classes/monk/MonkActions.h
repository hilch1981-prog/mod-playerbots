/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license.
 */

#ifndef _PLAYERBOT_MONKACTIONS_H
#define _PLAYERBOT_MONKACTIONS_H

#include "AiObject.h"
#include "GenericSpellActions.h"
#include "SharedDefines.h"

// Shared monk actions.
BUFF_ACTION(CastStanceOfTheSturdyOxAction, "stance of the sturdy ox");
BUFF_ACTION(CastStanceOfTheWiseSerpentAction, "stance of the wise serpent");
BUFF_ACTION(CastStanceOfTheFierceTigerAction, "stance of the fierce tiger");

MELEE_ACTION(CastJabAction, "jab");
MELEE_ACTION(CastTigerPalmAction, "tiger palm");
MELEE_ACTION(CastBlackoutKickAction, "blackout kick");
MELEE_ACTION(CastSpearHandStrikeAction, "spear hand strike");
HEAL_ACTION(CastExpelHarmAction, "expel harm");
BUFF_ACTION(CastFortifyingBrewAction, "fortifying brew");

// Brewmaster.
MELEE_ACTION(CastKegSmashAction, "keg smash");
BUFF_ACTION(CastPurifyingBrewAction, "purifying brew");
BUFF_ACTION(CastGuardAction, "guard");
BUFF_ACTION(CastElusiveBrewAction, "elusive brew");
SPELL_ACTION(CastProvokeAction, "provoke");

// Windwalker.
MELEE_ACTION(CastRisingSunKickAction, "rising sun kick");
MELEE_ACTION(CastFistsOfFuryAction, "fists of fury");
SPELL_ACTION(CastTouchOfKarmaAction, "touch of karma");

// Player-centred/AoE monk spells need a self target instead of the default
// "current target" used by CastSpellAction.
class CastSpinningCraneKickAction : public CastSpellAction
{
public:
    CastSpinningCraneKickAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "spinning crane kick") {}
    std::string const GetTargetName() override { return "self target"; }
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};

// Mistweaver direct/HoT heals. The Legends engine resolves spell IDs from
// 5.4.8 DBC data by name; no donor source implementation is copied here.
HEAL_PARTY_ACTION(CastSoothingMistOnPartyAction, "soothing mist", 15, HealingManaEfficiency::HIGH);
HEAL_PARTY_ACTION(CastSurgingMistOnPartyAction, "surging mist", 35, HealingManaEfficiency::LOW);
HEAL_PARTY_ACTION(CastEnvelopingMistOnPartyAction, "enveloping mist", 30, HealingManaEfficiency::MEDIUM);
HEAL_PARTY_ACTION(CastRenewingMistOnPartyAction, "renewing mist", 20, HealingManaEfficiency::HIGH);
HEAL_PARTY_ACTION(CastLifeCocoonOnPartyAction, "life cocoon", 45, HealingManaEfficiency::HIGH);

BUFF_ACTION(CastThunderFocusTeaAction, "thunder focus tea");
BUFF_ACTION(CastManaTeaAction, "mana tea");

class CastUpliftAction : public CastSpellAction
{
public:
    CastUpliftAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "uplift") {}
    std::string const GetTargetName() override { return "self target"; }
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};

class CastRevivalAction : public CastSpellAction
{
public:
    CastRevivalAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "revival") {}
    std::string const GetTargetName() override { return "self target"; }
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};

// Detox: keep typed action names so the cure strategy can independently
// schedule poison, disease and (Mistweaver-only) magic dispels.
class CastDetoxDiseaseAction : public CastCureSpellAction
{
public:
    CastDetoxDiseaseAction(PlayerbotAI* botAI) : CastCureSpellAction(botAI, "detox") {}
    std::string const getName() override { return "detox disease"; }
};

class CastDetoxPoisonAction : public CastCureSpellAction
{
public:
    CastDetoxPoisonAction(PlayerbotAI* botAI) : CastCureSpellAction(botAI, "detox") {}
    std::string const getName() override { return "detox poison"; }
};

class CastDetoxMagicAction : public CastCureSpellAction
{
public:
    CastDetoxMagicAction(PlayerbotAI* botAI) : CastCureSpellAction(botAI, "detox") {}
    std::string const getName() override { return "detox magic"; }
};

class CastDetoxDiseaseOnPartyAction : public CurePartyMemberAction
{
public:
    CastDetoxDiseaseOnPartyAction(PlayerbotAI* botAI) : CurePartyMemberAction(botAI, "detox", DISPEL_DISEASE) {}
    std::string const getName() override { return "detox disease on party"; }
};

class CastDetoxPoisonOnPartyAction : public CurePartyMemberAction
{
public:
    CastDetoxPoisonOnPartyAction(PlayerbotAI* botAI) : CurePartyMemberAction(botAI, "detox", DISPEL_POISON) {}
    std::string const getName() override { return "detox poison on party"; }
};

class CastDetoxMagicOnPartyAction : public CurePartyMemberAction
{
public:
    CastDetoxMagicOnPartyAction(PlayerbotAI* botAI) : CurePartyMemberAction(botAI, "detox", DISPEL_MAGIC) {}
    std::string const getName() override { return "detox magic on party"; }
};

#endif
