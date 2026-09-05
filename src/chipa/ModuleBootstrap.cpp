/*
 * Chipa PlayerBot V2 - MoP 5.4.8 Build 18414
 *
 * The module loader stays generic: it only registers Chipa integration
 * scripts. PlayerBot AI, session, database and game-policy code belongs behind
 * the compatibility adapter, not in the runtime Core or this bootstrap.
 */

void AddChipaPlayerbotUpdateScript();

void Addmod_playerbotsScripts()
{
    AddChipaPlayerbotUpdateScript();
}
