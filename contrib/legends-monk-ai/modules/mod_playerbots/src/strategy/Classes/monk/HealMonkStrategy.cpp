#include "HealMonkStrategy.h"

#include "Playerbots.h"

NextAction** HealMonkStrategy::getDefaultActions()
{
    // Healing reactions are trigger-driven. Keep only melee fallback so the
    // bot does not repeatedly attempt an expensive heal while the group is healthy.
    return NextAction::array(0, new NextAction("melee", ACTION_DEFAULT), nullptr);
}

void HealMonkStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericMonkStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "stance of the wise serpent",
        NextAction::array(0, new NextAction("stance of the wise serpent", ACTION_HIGH + 8), nullptr)));

    triggers.push_back(new TriggerNode(
        "party member to heal out of spell range",
        NextAction::array(0, new NextAction("reach party member to heal", ACTION_EMERGENCY + 3), nullptr)));

    triggers.push_back(new TriggerNode(
        "party member critical health",
        NextAction::array(
            0,
            new NextAction("life cocoon on party", ACTION_CRITICAL_HEAL + 7),
            new NextAction("surging mist on party", ACTION_CRITICAL_HEAL + 6),
            new NextAction("enveloping mist on party", ACTION_CRITICAL_HEAL + 5),
            nullptr)));

    triggers.push_back(new TriggerNode(
        "party member low health",
        NextAction::array(
            0,
            new NextAction("enveloping mist on party", ACTION_MEDIUM_HEAL + 5),
            new NextAction("surging mist on party", ACTION_MEDIUM_HEAL + 4),
            new NextAction("renewing mist on party", ACTION_MEDIUM_HEAL + 3),
            nullptr)));

    triggers.push_back(new TriggerNode(
        "party member medium health",
        NextAction::array(
            0,
            new NextAction("renewing mist on party", ACTION_LIGHT_HEAL + 8),
            new NextAction("soothing mist on party", ACTION_LIGHT_HEAL + 7),
            nullptr)));

    triggers.push_back(new TriggerNode(
        "party member almost full health",
        NextAction::array(0, new NextAction("renewing mist on party", ACTION_LIGHT_HEAL + 2), nullptr)));

    // Group-heal trigger mirrors existing Legends healer strategy semantics.
    // Revival is tried first; cooldown/availability failure falls through to Uplift.
    triggers.push_back(new TriggerNode(
        "medium group heal setting",
        NextAction::array(
            0,
            new NextAction("revival", ACTION_CRITICAL_HEAL + 8),
            new NextAction("thunder focus tea", ACTION_CRITICAL_HEAL + 6),
            new NextAction("uplift", ACTION_CRITICAL_HEAL + 5),
            nullptr)));

    triggers.push_back(new TriggerNode(
        "low mana",
        NextAction::array(0, new NextAction("mana tea", ACTION_HIGH + 4), nullptr)));
}
