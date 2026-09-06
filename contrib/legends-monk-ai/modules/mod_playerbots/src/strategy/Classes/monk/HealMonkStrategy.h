#ifndef _PLAYERBOT_HEALMONKSTRATEGY_H
#define _PLAYERBOT_HEALMONKSTRATEGY_H

#include "GenericMonkStrategy.h"

class PlayerbotAI;

class HealMonkStrategy : public GenericMonkStrategy
{
public:
    HealMonkStrategy(PlayerbotAI* botAI) : GenericMonkStrategy(botAI) {}

    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "heal"; }
};

#endif
