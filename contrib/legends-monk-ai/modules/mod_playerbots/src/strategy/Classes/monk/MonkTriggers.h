/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license.
 */

#ifndef _PLAYERBOT_MONKTRIGGERS_H
#define _PLAYERBOT_MONKTRIGGERS_H

#include "CureTriggers.h"
#include "GenericTriggers.h"
#include "Playerbots.h"
#include "PlayerbotAI.h"
#include "SharedDefines.h"

class PlayerbotAI;

BUFF_TRIGGER(StanceOfTheSturdyOxTrigger, "stance of the sturdy ox");
BUFF_TRIGGER(StanceOfTheWiseSerpentTrigger, "stance of the wise serpent");
BUFF_TRIGGER(StanceOfTheFierceTigerTrigger, "stance of the fierce tiger");

class TigerPowerTrigger : public BuffTrigger
{
public:
    TigerPowerTrigger(PlayerbotAI* botAI)
        : BuffTrigger(botAI, "tiger power", 1, true, true, 3000) {}
};

class ShuffleTrigger : public BuffTrigger
{
public:
    ShuffleTrigger(PlayerbotAI* botAI)
        : BuffTrigger(botAI, "shuffle", 1, true, true, 3000) {}
};

class ModerateStaggerTrigger : public Trigger
{
public:
    ModerateStaggerTrigger(PlayerbotAI* botAI) : Trigger(botAI, "moderate stagger") {}
    bool IsActive() override;
};

class HeavyStaggerTrigger : public Trigger
{
public:
    HeavyStaggerTrigger(PlayerbotAI* botAI) : Trigger(botAI, "heavy stagger") {}
    bool IsActive() override;
};

INTERRUPT_TRIGGER(SpearHandStrikeInterruptTrigger, "spear hand strike");

CURE_TRIGGER(DetoxDiseaseTrigger, "detox", DISPEL_DISEASE);
CURE_TRIGGER(DetoxPoisonTrigger, "detox", DISPEL_POISON);
CURE_PARTY_TRIGGER(DetoxDiseaseOnPartyTrigger, "detox", DISPEL_DISEASE);
CURE_PARTY_TRIGGER(DetoxPoisonOnPartyTrigger, "detox", DISPEL_POISON);

class DetoxMagicTrigger : public NeedCureTrigger
{
public:
    DetoxMagicTrigger(PlayerbotAI* botAI) : NeedCureTrigger(botAI, "detox", DISPEL_MAGIC) {}
    bool IsActive() override;
};

class DetoxMagicOnPartyTrigger : public PartyMemberNeedCureTrigger
{
public:
    DetoxMagicOnPartyTrigger(PlayerbotAI* botAI)
        : PartyMemberNeedCureTrigger(botAI, "detox", DISPEL_MAGIC) {}
    bool IsActive() override;
};

#endif
