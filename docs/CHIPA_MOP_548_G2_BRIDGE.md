# Chipa MoP 5.4.8 PlayerBot V2 - G2 Player Update Bridge Evidence

Status: implementation/preparation only. This document does **not** mark G2 PASS.

## Scope

G2 introduces the smallest generic update boundary needed to let the MoP runtime notify the PlayerBot module without moving PlayerBot manager, AI, session, or scheduling internals into the core.

Canonical flow:

`PlayerScript::OnUpdate(Player*, uint32)` -> Chipa dispatch -> compatibility-adapter callback -> module-owned PlayerBot manager/AI

The callback is intentionally null by default. Until a compatibility adapter explicitly registers a handler, dispatch is a safe no-op.

## Runtime API evidence

Runtime repository: `hilch1981-prog/MOP_V2_Repack`

Branch: `playerbot-v2-poc`

Verified runtime declaration:

- `src/server/game/Scripting/ScriptMgr.h`
- `PlayerScript::OnUpdate(Player* player, uint32 diff)`

The G2 script in `src/chipa/PlayerUpdateScript.cpp` overrides that exact MoP runtime signature.

## Module boundary

Adapted sources are isolated under `src/chipa`:

- `ModuleBootstrap.cpp`
- `PlayerUpdateBridge.h`
- `PlayerUpdateBridge.cpp`
- `PlayerUpdateScript.cpp`

`chipa_module.cmake` explicitly lists the three translation units that must be compiled by the Chipa runtime. The untouched upstream AzerothCore PlayerBot source tree remains excluded from the MoP build until individual components are adapted and verified.

The generic bridge contains no direct include of PlayerBot AI, manager, session, or database implementation headers. This keeps the runtime/core boundary generic and makes the later compatibility adapter the only place that should translate the MoP update event into PlayerBot-specific scheduling calls.

## Provenance

Initial bridge implementation:

- `268fa11cbd2dc90b59ac2d1c521c5c8a78021353` - `feat(chipa): add generic G2 player update bridge`

Build-manifest correction:

- `51a0434c8fdd67278f079089608d145648745dad` - `build(chipa): compile G2 player update bridge sources`

The manifest correction is required because `ModuleBootstrap.cpp` calls `AddChipaPlayerbotUpdateScript()`; omitting `PlayerUpdateScript.cpp` and `PlayerUpdateBridge.cpp` from the selected source list would leave the bridge implementation outside the runtime build.

## Gate discipline

G1 remains the current canonical gate. G1-B3 runtime boot and human/client smoke evidence is still required before G1 can be declared fully PASS.

Development of this G2 bridge may continue while that physical-environment validation is pending, but neither G1 nor G2 is promoted solely because this code exists or compiles.

Evidence required before G2 PASS includes, at minimum:

1. Runtime pins a reviewed module commit containing the bridge.
2. `MODULES=0` build remains green.
3. `MODULES=1` discovers and compiles the selected Chipa module sources and links `worldserver` successfully.
4. The compatibility adapter registers a PlayerBot-specific callback without introducing PlayerBot internals into the core.
5. Runtime/regression evidence confirms the update path behaves as intended.

## Next implementation step

After the current G1 build evidence is complete, pin the reviewed module commit in the runtime and run both module-off and module-on builds. Then implement the PlayerBot-specific compatibility adapter behind the registered callback, preserving the module's scheduler/update semantics rather than invoking internal AI implementation methods directly.
