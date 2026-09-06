#include "TankMonkStrategy.h"

#include "Playerbots.h"

NextAction** TankMonkStrategy::getDefaultActions()
{
    // MoP Brewmaster core: Keg Smash for energy/chi, Blackout Kick for
    // Shuffle, Jab as the low-priority chi builder.
    return NextAction::array(
        0,
        new NextAction("keg smash", ACTION_DEFAULT + 0.5f),
        new NextAction("blackout kick", ACTION_DEFAULT + 0.3f),
        new NextAction("jab", ACTION_DEFAULT + 0.2f),
        new NextAction("melee", ACTION_DEFAULT),
        nullptr);
}

void TankMonkStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericMonkStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "stance of the sturdy ox",
        NextAction::array(0, new NextAction("stance of the sturdy ox", ACTION_HIGH + 8), nullptr)));

    // Maintain Shuffle/Tiger Power rather than spamming their generators.
    triggers.push_back(new TriggerNode(
        "shuffle",
        NextAction::array(0, new NextAction("blackout kick", ACTION_HIGH + 6), nullptr)));
    triggers.push_back(new TriggerNode(
        "tiger power",
        NextAction::array(0, new NextAction("tiger palm", ACTION_HIGH + 5), nullptr)));

    // Heavy stagger is intentionally above moderate stagger.
    triggers.push_back(new TriggerNode(
        "heavy stagger",
        NextAction::array(0, new NextAction("purifying brew", ACTION_INTERRUPT + 2), nullptr)));
    triggers.push_back(new TriggerNode(
        "moderate stagger",
        NextAction::array(0, new NextAction("purifying brew", ACTION_HIGH + 4), nullptr)));

    triggers.push_back(new TriggerNode(
        "lose aggro",
        NextAction::array(0, new NextAction("provoke", ACTION_INTERRUPT + 1), nullptr)));

    triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(
            0,
            new NextAction("guard", ACTION_EMERGENCY + 2),
            new NextAction("elusive brew", ACTION_EMERGENCY + 1),
            nullptr)));
}
