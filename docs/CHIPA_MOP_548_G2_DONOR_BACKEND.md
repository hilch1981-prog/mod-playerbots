# Chipa MoP 5.4.8 G2 donor backend mapping

Status: **preparatory design evidence only**. This document does not promote G1 or G2.

## Purpose

The generic Chipa G2 bridge already exposes three callbacks:

- `IsManagedPlayerCallback`
- `PlayerAiUpdateCallback`
- `PlayerMgrUpdateCallback`

A later donor-specific compatibility translation unit will bind those callbacks to the imported PlayerBot implementation. The concrete PlayerBot headers and ownership logic must remain outside the generic Core-facing bridge.

## Verified donor ownership API

`PlayerbotsMgr` owns two per-player maps and exposes direct null-safe accessors:

- `PlayerbotsMgr::instance().GetPlayerbotAI(player)` -> `_playerbotsAIMap`
- `PlayerbotsMgr::instance().GetPlayerbotMgr(player)` -> `_playerbotsMgrMap`

This is important for the MoP port because the legacy donor macro `GET_PLAYERBOT_MGR(player)` resolves through `player->GetSession()->GetPlayerbotMgr()`, which assumes an AzerothCore-specific `WorldSession` core patch. Chipa G2 must not add that dependency merely to schedule updates.

## Planned compatibility binding

The donor-specific translation unit should implement the callback semantics below without caching PlayerBot-owned raw pointers across callback invocations:

```text
IsManagedPlayer(player):
    return GetPlayerbotAI(player) != null OR GetPlayerbotMgr(player) != null

PlayerAiUpdate(player, diff):
    ai = GetPlayerbotAI(player)
    if ai != null:
        ai->UpdateAI(diff)

PlayerMgrUpdate(player, diff):
    mgr = GetPlayerbotMgr(player)
    if mgr != null:
        mgr->UpdateAI(diff)
```

The generic adapter already guarantees the dispatch order:

1. reject null `Player*`
2. reject ordinary/unmanaged players
3. AI callback
4. manager callback

The concrete binding must preserve the donor public scheduling contract `PlayerbotAI::UpdateAI(diff)` -> `PlayerbotMgr::UpdateAI(diff)`. It must never call `UpdateAIInternal()` directly.

## Lifetime rule

The direct accessors return pointers owned by `PlayerbotsMgr`. The compatibility layer must perform a fresh lookup inside each work callback and must not retain those pointers across ticks, login/logout transitions, backend replacement, or SelfBot enable/disable transitions.

The current generic backend publication mechanism protects callback-set coherence only. It intentionally does **not** claim to extend the lifetime of donor-owned AI/manager objects.

## Integration boundary

Until the relevant donor implementation sources compile against the SkyFire 5.4.8 runtime, the concrete donor translation unit must not be added to `chipa_module.cmake`. Bringing donor headers into the generic bridge to bypass this gate is prohibited.

When the concrete TU is enabled, acceptance requires all of the following evidence:

- generic boundary remains free of concrete PlayerBot/session/database headers;
- donor direct-map lookup contract still passes;
- `MODULES=0` full server build passes;
- `MODULES=1` full server build and module discovery pass;
- Claude Code Review, when available, has no unresolved blocker-level finding;
- no G2 PASS is declared until the canonical preceding runtime gate evidence is satisfied.

## Next compatibility work

Port the minimum donor source closure needed by `PlayerbotsMgr`, `PlayerbotAI::UpdateAI`, and `PlayerbotMgr::UpdateAI` as an isolated compatibility slice. Resolve SkyFire/AzerothCore type and API differences inside that slice instead of expanding Core-facing Chipa interfaces.
