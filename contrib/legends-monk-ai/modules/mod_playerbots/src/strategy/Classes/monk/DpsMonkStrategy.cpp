#include "DpsMonkStrategy.h"

#include "Playerbots.h"

NextAction** DpsMonkStrategy::getDefaultActions()
{
    return NextAction::array(
        0,
        new NextAction("rising sun kick", ACTION_DEFAULT + 0.5f),
        new NextAction("fists of fury", ACTION_DEFAULT + 0.4f),
        new NextAction("blackout kick", ACTION_DEFAULT + 0.3f),
        new NextAction("jab", ACTION_DEFAULT + 0.1f),
        new NextAction("melee", ACTION_DEFAULT),
        nullptr);
}

void DpsMonkStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericMonkStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "stance of the fierce tiger",
        NextAction::array(0, new NextAction("stance of the fierce tiger", ACTION_HIGH + 8), nullptr)));

    triggers.push_back(new TriggerNode(
        "tiger power",
        NextAction::array(0, new NextAction("tiger palm", ACTION_HIGH + 5), nullptr)));

    triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("touch of karma", ACTION_EMERGENCY + 1), nullptr)));
}
