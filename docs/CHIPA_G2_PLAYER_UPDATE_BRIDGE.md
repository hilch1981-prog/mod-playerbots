# Chipa PlayerBot V2 - G2 player update bridge

Status: implementation preparation only; **not a G2 PASS claim**.

## Purpose

The MoP 5.4.8 runtime already exposes the generic `PlayerScript::OnUpdate(Player*, uint32)` hook. The imported AzerothCore PlayerBot source uses its own `PlayerbotsPlayerScript::OnPlayerAfterUpdate` to drive `PlayerbotAI::UpdateAI(diff)` and `PlayerbotMgr::UpdateAI(diff)`. Porting that AzerothCore script directly would pull incompatible hook-registration APIs and PlayerBot-specific headers into the runtime integration boundary.

G2 therefore starts with a narrow module-owned seam:

`SkyFire PlayerScript::OnUpdate -> Chipa DispatchPlayerUpdate -> compatibility adapter callback -> PlayerBot manager/AI`

The first three components are implemented under `src/chipa`. The final adapter callback is intentionally not installed yet.

## Architecture constraints

- No Core file includes PlayerBot manager/AI/session/DB headers.
- `ModuleBootstrap.cpp` registers only Chipa integration scripts.
- `PlayerUpdateScript.cpp` depends only on the runtime scripting API plus the narrow bridge header.
- `PlayerUpdateBridge` is null-safe. Until an adapter registers a callback, player updates are a no-op.
- The callback is stored atomically because registration and player updates must not rely on an undocumented single-thread assumption.
- Imported AzerothCore `OnPlayerAfterUpdate` is reference behavior only; its hook-array constructor and manager macros are not copied into SkyFire.

## Evidence required before G2 PASS

1. Runtime pins a reviewed module commit containing this bridge.
2. `MODULES=0` still builds cleanly.
3. `MODULES=1` compiles and links the registered `ChipaPlayerbotUpdateScript`.
4. Static review confirms the Core remains PlayerBot-header-free.
5. A real compatibility adapter registers a callback that reaches the preserved PlayerBot scheduling entry point rather than bypassing it.
6. Runtime evidence proves update dispatch with PlayerBot enabled and no human-player regression.

Until those checks are complete, this bridge is only G2 preparation and G1-B3 remains independently pending.
