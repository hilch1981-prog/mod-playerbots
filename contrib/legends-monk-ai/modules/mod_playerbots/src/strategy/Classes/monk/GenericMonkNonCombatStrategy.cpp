#include "GenericMonkNonCombatStrategy.h"

#include "Playerbots.h"

GenericMonkNonCombatStrategy::GenericMonkNonCombatStrategy(PlayerbotAI* botAI)
    : NonCombatStrategy(botAI)
{
}

void GenericMonkNonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    NonCombatStrategy::InitTriggers(triggers);
}
