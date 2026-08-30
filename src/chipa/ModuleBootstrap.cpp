/*
 * Chipa PlayerBot V2 - MoP 5.4.8 Build 18414
 *
 * POC-G1 bootstrap only.
 *
 * This translation unit deliberately contains no PlayerBot AI, rotation,
 * session, database or world-policy logic. Its only job is to prove that the
 * Chipa runtime can discover, compile, link and invoke an optional module
 * loader without creating a Core -> PlayerBot header dependency.
 */

void Addmod_playerbotsScripts()
{
    // POC-G1: intentionally empty. POC-G2 will register Chipa integration
    // scripts here after the generic module loader has passed build/boot gates.
}
