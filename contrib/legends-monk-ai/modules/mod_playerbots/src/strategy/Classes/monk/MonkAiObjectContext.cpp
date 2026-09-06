#include "MonkAiObjectContext.h"

#include "DpsMonkStrategy.h"
#include "GenericMonkNonCombatStrategy.h"
#include "GenericMonkStrategy.h"
#include "HealMonkStrategy.h"
#include "MonkActions.h"
#include "MonkTriggers.h"
#include "NamedObjectContext.h"
#include "Playerbots.h"
#include "TankMonkStrategy.h"

class MonkStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    MonkStrategyFactoryInternal()
    {
        creators["nc"] = &MonkStrategyFactoryInternal::nc;
        creators["aoe"] = &MonkStrategyFactoryInternal::aoe;
        creators["melee aoe"] = &MonkStrategyFactoryInternal::melee_aoe;
        creators["cure"] = &MonkStrategyFactoryInternal::cure;
    }

private:
    static Strategy* nc(PlayerbotAI* botAI) { return new GenericMonkNonCombatStrategy(botAI); }
    static Strategy* aoe(PlayerbotAI* botAI) { return new MonkAoeStrategy(botAI); }
    static Strategy* melee_aoe(PlayerbotAI* botAI) { return new MonkMeleeAoeStrategy(botAI); }
    static Strategy* cure(PlayerbotAI* botAI) { return new MonkCureStrategy(botAI); }
};

class MonkCombatStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    MonkCombatStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
    {
        creators["tank"] = &MonkCombatStrategyFactoryInternal::tank;
        creators["heal"] = &MonkCombatStrategyFactoryInternal::heal;
        creators["melee"] = &MonkCombatStrategyFactoryInternal::melee;
        creators["dps"] = &MonkCombatStrategyFactoryInternal::melee;
    }

private:
    static Strategy* tank(PlayerbotAI* botAI) { return new TankMonkStrategy(botAI); }
    static Strategy* heal(PlayerbotAI* botAI) { return new HealMonkStrategy(botAI); }
    static Strategy* melee(PlayerbotAI* botAI) { return new DpsMonkStrategy(botAI); }
};

class MonkTriggerFactoryInternal : public NamedObjectContext<Trigger>
{
public:
    MonkTriggerFactoryInternal()
    {
        creators["stance of the sturdy ox"] = &MonkTriggerFactoryInternal::sturdy_ox;
        creators["stance of the wise serpent"] = &MonkTriggerFactoryInternal::wise_serpent;
        creators["stance of the fierce tiger"] = &MonkTriggerFactoryInternal::fierce_tiger;
        creators["tiger power"] = &MonkTriggerFactoryInternal::tiger_power;
        creators["shuffle"] = &MonkTriggerFactoryInternal::shuffle;
        creators["moderate stagger"] = &MonkTriggerFactoryInternal::moderate_stagger;
        creators["heavy stagger"] = &MonkTriggerFactoryInternal::heavy_stagger;
        creators["spear hand strike"] = &MonkTriggerFactoryInternal::spear_hand_strike;

        creators["detox disease"] = &MonkTriggerFactoryInternal::detox_disease;
        creators["detox disease on party"] = &MonkTriggerFactoryInternal::detox_disease_party;
        creators["detox poison"] = &MonkTriggerFactoryInternal::detox_poison;
        creators["detox poison on party"] = &MonkTriggerFactoryInternal::detox_poison_party;
        creators["detox magic"] = &MonkTriggerFactoryInternal::detox_magic;
        creators["detox magic on party"] = &MonkTriggerFactoryInternal::detox_magic_party;
    }

private:
    static Trigger* sturdy_ox(PlayerbotAI* botAI) { return new StanceOfTheSturdyOxTrigger(botAI); }
    static Trigger* wise_serpent(PlayerbotAI* botAI) { return new StanceOfTheWiseSerpentTrigger(botAI); }
    static Trigger* fierce_tiger(PlayerbotAI* botAI) { return new StanceOfTheFierceTigerTrigger(botAI); }
    static Trigger* tiger_power(PlayerbotAI* botAI) { return new TigerPowerTrigger(botAI); }
    static Trigger* shuffle(PlayerbotAI* botAI) { return new ShuffleTrigger(botAI); }
    static Trigger* moderate_stagger(PlayerbotAI* botAI) { return new ModerateStaggerTrigger(botAI); }
    static Trigger* heavy_stagger(PlayerbotAI* botAI) { return new HeavyStaggerTrigger(botAI); }
    static Trigger* spear_hand_strike(PlayerbotAI* botAI) { return new SpearHandStrikeInterruptTrigger(botAI); }

    static Trigger* detox_disease(PlayerbotAI* botAI) { return new DetoxDiseaseTrigger(botAI); }
    static Trigger* detox_disease_party(PlayerbotAI* botAI) { return new DetoxDiseaseOnPartyTrigger(botAI); }
    static Trigger* detox_poison(PlayerbotAI* botAI) { return new DetoxPoisonTrigger(botAI); }
    static Trigger* detox_poison_party(PlayerbotAI* botAI) { return new DetoxPoisonOnPartyTrigger(botAI); }
    static Trigger* detox_magic(PlayerbotAI* botAI) { return new DetoxMagicTrigger(botAI); }
    static Trigger* detox_magic_party(PlayerbotAI* botAI) { return new DetoxMagicOnPartyTrigger(botAI); }
};

class MonkAiObjectContextInternal : public NamedObjectContext<Action>
{
public:
    MonkAiObjectContextInternal()
    {
        creators["stance of the sturdy ox"] = &MonkAiObjectContextInternal::sturdy_ox;
        creators["stance of the wise serpent"] = &MonkAiObjectContextInternal::wise_serpent;
        creators["stance of the fierce tiger"] = &MonkAiObjectContextInternal::fierce_tiger;

        creators["jab"] = &MonkAiObjectContextInternal::jab;
        creators["tiger palm"] = &MonkAiObjectContextInternal::tiger_palm;
        creators["blackout kick"] = &MonkAiObjectContextInternal::blackout_kick;
        creators["expel harm"] = &MonkAiObjectContextInternal::expel_harm;
        creators["spear hand strike"] = &MonkAiObjectContextInternal::spear_hand_strike;
        creators["fortifying brew"] = &MonkAiObjectContextInternal::fortifying_brew;
        creators["spinning crane kick"] = &MonkAiObjectContextInternal::spinning_crane_kick;

        creators["keg smash"] = &MonkAiObjectContextInternal::keg_smash;
        creators["purifying brew"] = &MonkAiObjectContextInternal::purifying_brew;
        creators["guard"] = &MonkAiObjectContextInternal::guard;
        creators["elusive brew"] = &MonkAiObjectContextInternal::elusive_brew;
        creators["provoke"] = &MonkAiObjectContextInternal::provoke;

        creators["rising sun kick"] = &MonkAiObjectContextInternal::rising_sun_kick;
        creators["fists of fury"] = &MonkAiObjectContextInternal::fists_of_fury;
        creators["touch of karma"] = &MonkAiObjectContextInternal::touch_of_karma;

        creators["soothing mist on party"] = &MonkAiObjectContextInternal::soothing_mist_party;
        creators["surging mist on party"] = &MonkAiObjectContextInternal::surging_mist_party;
        creators["enveloping mist on party"] = &MonkAiObjectContextInternal::enveloping_mist_party;
        creators["renewing mist on party"] = &MonkAiObjectContextInternal::renewing_mist_party;
        creators["life cocoon on party"] = &MonkAiObjectContextInternal::life_cocoon_party;
        creators["thunder focus tea"] = &MonkAiObjectContextInternal::thunder_focus_tea;
        creators["mana tea"] = &MonkAiObjectContextInternal::mana_tea;
        creators["uplift"] = &MonkAiObjectContextInternal::uplift;
        creators["revival"] = &MonkAiObjectContextInternal::revival;

        creators["detox disease"] = &MonkAiObjectContextInternal::detox_disease;
        creators["detox disease on party"] = &MonkAiObjectContextInternal::detox_disease_party;
        creators["detox poison"] = &MonkAiObjectContextInternal::detox_poison;
        creators["detox poison on party"] = &MonkAiObjectContextInternal::detox_poison_party;
        creators["detox magic"] = &MonkAiObjectContextInternal::detox_magic;
        creators["detox magic on party"] = &MonkAiObjectContextInternal::detox_magic_party;
    }

private:
    static Action* sturdy_ox(PlayerbotAI* botAI) { return new CastStanceOfTheSturdyOxAction(botAI); }
    static Action* wise_serpent(PlayerbotAI* botAI) { return new CastStanceOfTheWiseSerpentAction(botAI); }
    static Action* fierce_tiger(PlayerbotAI* botAI) { return new CastStanceOfTheFierceTigerAction(botAI); }

    static Action* jab(PlayerbotAI* botAI) { return new CastJabAction(botAI); }
    static Action* tiger_palm(PlayerbotAI* botAI) { return new CastTigerPalmAction(botAI); }
    static Action* blackout_kick(PlayerbotAI* botAI) { return new CastBlackoutKickAction(botAI); }
    static Action* expel_harm(PlayerbotAI* botAI) { return new CastExpelHarmAction(botAI); }
    static Action* spear_hand_strike(PlayerbotAI* botAI) { return new CastSpearHandStrikeAction(botAI); }
    static Action* fortifying_brew(PlayerbotAI* botAI) { return new CastFortifyingBrewAction(botAI); }
    static Action* spinning_crane_kick(PlayerbotAI* botAI) { return new CastSpinningCraneKickAction(botAI); }

    static Action* keg_smash(PlayerbotAI* botAI) { return new CastKegSmashAction(botAI); }
    static Action* purifying_brew(PlayerbotAI* botAI) { return new CastPurifyingBrewAction(botAI); }
    static Action* guard(PlayerbotAI* botAI) { return new CastGuardAction(botAI); }
    static Action* elusive_brew(PlayerbotAI* botAI) { return new CastElusiveBrewAction(botAI); }
    static Action* provoke(PlayerbotAI* botAI) { return new CastProvokeAction(botAI); }

    static Action* rising_sun_kick(PlayerbotAI* botAI) { return new CastRisingSunKickAction(botAI); }
    static Action* fists_of_fury(PlayerbotAI* botAI) { return new CastFistsOfFuryAction(botAI); }
    static Action* touch_of_karma(PlayerbotAI* botAI) { return new CastTouchOfKarmaAction(botAI); }

    static Action* soothing_mist_party(PlayerbotAI* botAI) { return new CastSoothingMistOnPartyAction(botAI); }
    static Action* surging_mist_party(PlayerbotAI* botAI) { return new CastSurgingMistOnPartyAction(botAI); }
    static Action* enveloping_mist_party(PlayerbotAI* botAI) { return new CastEnvelopingMistOnPartyAction(botAI); }
    static Action* renewing_mist_party(PlayerbotAI* botAI) { return new CastRenewingMistOnPartyAction(botAI); }
    static Action* life_cocoon_party(PlayerbotAI* botAI) { return new CastLifeCocoonOnPartyAction(botAI); }
    static Action* thunder_focus_tea(PlayerbotAI* botAI) { return new CastThunderFocusTeaAction(botAI); }
    static Action* mana_tea(PlayerbotAI* botAI) { return new CastManaTeaAction(botAI); }
    static Action* uplift(PlayerbotAI* botAI) { return new CastUpliftAction(botAI); }
    static Action* revival(PlayerbotAI* botAI) { return new CastRevivalAction(botAI); }

    static Action* detox_disease(PlayerbotAI* botAI) { return new CastDetoxDiseaseAction(botAI); }
    static Action* detox_disease_party(PlayerbotAI* botAI) { return new CastDetoxDiseaseOnPartyAction(botAI); }
    static Action* detox_poison(PlayerbotAI* botAI) { return new CastDetoxPoisonAction(botAI); }
    static Action* detox_poison_party(PlayerbotAI* botAI) { return new CastDetoxPoisonOnPartyAction(botAI); }
    static Action* detox_magic(PlayerbotAI* botAI) { return new CastDetoxMagicAction(botAI); }
    static Action* detox_magic_party(PlayerbotAI* botAI) { return new CastDetoxMagicOnPartyAction(botAI); }
};

MonkAiObjectContext::MonkAiObjectContext(PlayerbotAI* botAI) : AiObjectContext(botAI)
{
    strategyContexts.Add(new MonkStrategyFactoryInternal());
    strategyContexts.Add(new MonkCombatStrategyFactoryInternal());
    actionContexts.Add(new MonkAiObjectContextInternal());
    triggerContexts.Add(new MonkTriggerFactoryInternal());
}
