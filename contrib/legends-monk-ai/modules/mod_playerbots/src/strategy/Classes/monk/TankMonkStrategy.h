#ifndef _PLAYERBOT_TANKMONKSTRATEGY_H
#define _PLAYERBOT_TANKMONKSTRATEGY_H

#include "GenericMonkStrategy.h"

class PlayerbotAI;

class TankMonkStrategy : public GenericMonkStrategy
{
public:
    TankMonkStrategy(PlayerbotAI* botAI) : GenericMonkStrategy(botAI) {}

    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "tank"; }
};

#endif
