#ifndef _PLAYERBOT_DPSMONKSTRATEGY_H
#define _PLAYERBOT_DPSMONKSTRATEGY_H

#include "GenericMonkStrategy.h"

class PlayerbotAI;

class DpsMonkStrategy : public GenericMonkStrategy
{
public:
    DpsMonkStrategy(PlayerbotAI* botAI) : GenericMonkStrategy(botAI) {}

    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "melee"; }
};

#endif
