# Chipa PlayerBot V2 API Compatibility Contract

The compatibility layer isolates external PlayerBot APIs from the Chipa MoP 5.4.8 Core without pretending the Core is AzerothCore.

## Design rule

The Adapter is a **selective abstraction / volatility firewall**.

Stable Core types may be used directly where safe:

- `Player*`
- `Unit*`
- `Group*`
- `ObjectGuid`
- `Position`
- `SpellInfo const*`

Only unstable/core-specific operations are abstracted.

## Planned compatibility areas

```text
compat/
├─ BotCompatCommon.h
├─ BotCapabilities.h
├─ BotSessionCompat.h/.cpp
├─ BotLoginCompat.h/.cpp
├─ BotMovementCompat.h/.cpp
├─ BotSpellCompat.h/.cpp
├─ BotGroupCompat.h/.cpp
├─ BotLfgCompat.h/.cpp
├─ BotCommandCompat.h/.cpp
├─ BotConfigCompat.h/.cpp
├─ BotLocaleCompat.h/.cpp
└─ BotDatabaseCompat.h/.cpp
```

Names are implementation-provisional; responsibilities are architectural.

## Integration layer

Core script callbacks are normalized before entering PlayerBot logic.

```text
integration/
├─ PlayerbotPlayerScript.cpp
├─ PlayerbotWorldScript.cpp
└─ PlayerbotCommandScript.cpp
```

Examples of differences that belong here:

- Chipa vs Digi PlayerScript callback names
- command security model differences
- config-load callback wiring

## Session compatibility

Centralize socketless `WorldSession` creation/default arguments. Session ownership remains with the PlayerBot lifecycle/manager layer, not the Adapter.

## Login compatibility

Expose a typed bot-login result such as Pending/Success/Failed rather than a boolean-only contract.

Async requests carry stable IDs (GUID/account/request/generation), not long-lived raw game-object pointers.

## Movement compatibility

The movement adapter executes movement/core-finalization mechanics. AI/Strategy decides **where/why** to move.

SelfBot control policy can suppress movement actions while still using the same AI engine.

## Spell compatibility

Separate mechanics from AI policy:

- `BotSpellCompat` → Core-specific lookup/cast/access differences
- Spell/Combat Service → legality, GCD, range, LoS, resource, aura-refresh semantics
- Class/Spec AI → priority and spell choice

MoP numeric spell IDs are canonical. String/name lookup exists only for compatibility/legacy resolution and must not drive hot-path combat decisions.

## Group compatibility

Group adapters return facts such as target-icon/group state. They do not decide tank/assist/target policy.

## LFG compatibility

LFG is an optional capability. Missing or incompatible LFG access disables LFG-related behavior rather than breaking basic PlayerBot operation.

## Commands and security

Do not port AzerothCore RBAC into Chipa Core just to compile external PlayerBot code.

Separate:

- **Admin command permission** — who may configure/control the module
- **Bot order authorization** — which player/master may order a given bot

Map neutral module permissions onto Chipa's native security model.

## Config compatibility

Use Chipa's native config system and load a validated PlayerBot runtime snapshot. AI hot paths do not repeatedly call ConfigMgr.

## Database compatibility

AI/engine code never sees SQL/QueryResult implementation details.

```text
AI / Service
   ↓
Repository
   ↓
BotDatabaseCompat
   ↓
DB layer
```

DB worker callbacks must not mutate live `Player`, `Map`, Group or Spell state.

## Locale compatibility

- numeric IDs define AI semantics
- enUS may remain the canonical compatibility locale for legacy upstream name resolution
- display text may use user locale such as koKR
- localization must never change AI meaning

## Capability and fail-soft model

Capabilities may be Required or Optional.

Examples:

- required for SelfBot POC: Script/Command/Config/Spell basics
- later required for ManagedBot: Session/Login/Teleport
- optional depending feature: LFG, Travel dataset, speech, PlayerBot DB

Missing optional capabilities disable only their dependent feature.

## Thread-affinity contract

World/game-thread work:

- Player state mutation
- spell cast
- movement
- group/map mutation
- teleport finalization
- action execution

Worker-safe candidates only when based on immutable/DTO inputs:

- DB queries
- text/data processing
- high-level static travel graph search
- scoring/serialization

## Compatibility version

The module should expose/check a small compatibility API version. A mismatch must fail clearly instead of silently using incompatible assumptions.
