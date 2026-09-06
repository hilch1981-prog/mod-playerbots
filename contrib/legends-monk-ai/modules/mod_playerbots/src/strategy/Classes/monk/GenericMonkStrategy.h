#ifndef _PLAYERBOT_GENERICMONKSTRATEGY_H
#define _PLAYERBOT_GENERICMONKSTRATEGY_H

#include "CombatStrategy.h"

class PlayerbotAI;

class GenericMonkStrategy : public CombatStrategy
{
public:
    GenericMonkStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "monk"; }
};

class MonkAoeStrategy : public CombatStrategy
{
public:
    MonkAoeStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "aoe"; }
};

class MonkMeleeAoeStrategy : public MonkAoeStrategy
{
public:
    MonkMeleeAoeStrategy(PlayerbotAI* botAI) : MonkAoeStrategy(botAI) {}
    std::string const getName() override { return "melee aoe"; }
};

class MonkCureStrategy : public Strategy
{
public:
    MonkCureStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "cure"; }
};

#endif
