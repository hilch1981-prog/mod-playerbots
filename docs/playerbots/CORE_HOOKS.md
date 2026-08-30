# Chipa PlayerBot V2 Core Hooks

This document records the only kinds of Chipa Core changes the PlayerBot V2 architecture permits.

## Principle

Core may expose generic infrastructure required for a bot to exist as a real `Player`. PlayerBot behavior/policy remains in the module.

## Allowed Core bridge areas

### 1. Generic module loading

- compile-time module switch
- generic module discovery/build
- generated/central module script loader
- no PlayerBot-specific dependency from Core game libraries

### 2. PlayerScript update reuse

Use the existing PlayerScript update callback to drive the module manager. Do not add a second Core bot AI loop.

### 3. Socketless WorldSession safety

Managed/Random bots use a real `WorldSession` without a network socket.

Allowed changes:

- generic bot/session marker if needed (`IsBot`/`SetBot` style)
- null-socket guards where Core unconditionally dereferences the socket
- generic safe behavior for outgoing packets with no client

Forbidden:

- Core owning `PlayerbotAI`
- ordinary World network session registration solely to make bots work

### 4. Server-side bot character login

Character query/load internals remain Core-owned.

The module may call a small server-side login bridge that:

- validates account/character ownership
- starts the existing DB login query set
- completes `Player::LoadFromDB` and world insertion on the appropriate game thread
- reports Pending/Success/Failed without requiring a fake client login packet

Bulk login must be asynchronous/paced.

### 5. Server-side teleport finalization

Do not synthesize fake client ACK packets.

- Far teleport: reuse an existing server-side worldport finalization path where available.
- Near teleport: factor/reuse the internal position/zone/pet/delayed-op finalization so a bot can complete it server-side.

### 6. Small generic read helpers

If module code cannot safely observe required Core state, a small generic read-only helper is acceptable.

Examples considered acceptable in later phases:

- read a group target icon
- read active LFG proposal/state if no suitable public accessor exists

Helpers must be generic (`GetTargetIcon`) rather than bot-specific (`GetBotTargetIcon`).

## Explicitly forbidden Core changes

- class/spec AI or rotations
- combat priority tables
- Quest/Travel/LFG/RandomBot policy
- SelfBot-specific movement/rotation logic
- official AzerothCore PlayerBot API emulation solely to avoid writing adapters
- WotLK spell/talent/packet behavior introduced without MoP validation
- reverse dependency from Core game libraries to module headers

## Core modification budget

For each feature:

- **0 Core files** — preferred
- **Adapter-only** — normal
- **1 small generic Core helper** — acceptable with justification
- **multiple bot-specific Core changes** — architecture review required

## POC expected Core touch points

The first SelfBot POC should primarily require build/module registration and existing PlayerScript hooks. Socketless session/login/teleport bridge work belongs to the later ManagedBot gate.

## Regression requirement

Every Core bridge PR must verify:

1. PlayerBot active
2. PlayerBot compiled but disabled
3. PlayerBot/module absent where build configuration allows

Human login, movement, teleport, combat, group, quest and shutdown behavior must remain correct.
