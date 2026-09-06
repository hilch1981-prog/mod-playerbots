/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license.
 */

#ifndef _PLAYERBOT_GENERICMONKNONCOMBATSTRATEGY_H
#define _PLAYERBOT_GENERICMONKNONCOMBATSTRATEGY_H

#include "NonCombatStrategy.h"

class PlayerbotAI;

class GenericMonkNonCombatStrategy : public NonCombatStrategy
{
public:
    GenericMonkNonCombatStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "nc"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
