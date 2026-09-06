/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license.
 */

#ifndef _PLAYERBOT_MONKAIOBJECTCONTEXT_H
#define _PLAYERBOT_MONKAIOBJECTCONTEXT_H

#include "AiObjectContext.h"

class PlayerbotAI;

class MonkAiObjectContext : public AiObjectContext
{
public:
    MonkAiObjectContext(PlayerbotAI* botAI);
};

#endif
