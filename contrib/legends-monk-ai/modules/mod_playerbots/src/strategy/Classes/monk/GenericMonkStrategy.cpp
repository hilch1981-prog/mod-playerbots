#include "GenericMonkStrategy.h"

#include "Playerbots.h"

class GenericMonkStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericMonkStrategyActionNodeFactory()
    {
        creators["jab"] = &jab;
        creators["tiger palm"] = &tiger_palm;
        creators["blackout kick"] = &blackout_kick;
        creators["expel harm"] = &expel_harm;
        creators["spear hand strike"] = &spear_hand_strike;
    }

private:
    ACTION_NODE(jab, "jab");
    ACTION_NODE(tiger_palm, "tiger palm");
    ACTION_NODE(blackout_kick, "blackout kick");
    ACTION_NODE(expel_harm, "expel harm");
    ACTION_NODE(spear_hand_strike, "spear hand strike");
};

GenericMonkStrategy::GenericMonkStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI)
{
    actionNodeFactories.Add(new GenericMonkStrategyActionNodeFactory());
}

void GenericMonkStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "spear hand strike",
        NextAction::array(0, new NextAction("spear hand strike", ACTION_INTERRUPT), nullptr)));

    triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("expel harm", ACTION_HIGH + 2), nullptr)));

    triggers.push_back(new TriggerNode(
        "critical health",
        NextAction::array(0, new NextAction("fortifying brew", ACTION_EMERGENCY), nullptr)));
}

void MonkAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("spinning crane kick", ACTION_HIGH), nullptr)));
    triggers.push_back(new TriggerNode(
        "medium aoe",
        NextAction::array(0, new NextAction("spinning crane kick", ACTION_HIGH + 1), nullptr)));
}

void MonkCureStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "detox disease",
        NextAction::array(0, new NextAction("detox disease", ACTION_DISPEL + 2), nullptr)));
    triggers.push_back(new TriggerNode(
        "detox disease on party",
        NextAction::array(0, new NextAction("detox disease on party", ACTION_DISPEL + 1), nullptr)));

    triggers.push_back(new TriggerNode(
        "detox poison",
        NextAction::array(0, new NextAction("detox poison", ACTION_DISPEL + 2), nullptr)));
    triggers.push_back(new TriggerNode(
        "detox poison on party",
        NextAction::array(0, new NextAction("detox poison on party", ACTION_DISPEL + 1), nullptr)));

    // Detox gains friendly magic dispel functionality for Mistweaver only.
    triggers.push_back(new TriggerNode(
        "detox magic",
        NextAction::array(0, new NextAction("detox magic", ACTION_DISPEL + 2), nullptr)));
    triggers.push_back(new TriggerNode(
        "detox magic on party",
        NextAction::array(0, new NextAction("detox magic on party", ACTION_DISPEL + 1), nullptr)));
}
