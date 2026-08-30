# Chipa PlayerBot V2 Architecture v0.1

Target: World of Warcraft: Mists of Pandaria 5.4.8 (Build 18414)

Status: **Architecture baseline complete — 8/8 parts**

This document is the consolidated architecture baseline for the Chipa PlayerBot V2 project. It converts the eight design parts into a durable implementation reference for the `mop-5.4.8-v2` branch.

## 1. Architecture principles and responsibility boundaries

### Final source of truth

`hilch1981-prog/MOP_V2_Repack` remains the final runtime Core source of truth. External repositories are donors and references only.

### Layering

```text
Chipa Core
   ↓
Minimal Core Bridge
   ↓
Compatibility Adapter
   ↓
PlayerBot Engine
   ↓
Functional Services
   ↓
Class / Spec AI
```

### Core purity

Core may contain only bot-existence infrastructure:

- generic module loading
- script registration
- socketless `WorldSession` support
- null-socket safety
- server-side bot character login/logout bridge
- server-side teleport finalization
- small generic read helpers where unavoidable

Core must not contain:

- PlayerBot AI policy
- class/spec rotations
- Quest/Travel/LFG policy
- RandomBot policy
- SelfBot-specific rotation
- combat priorities

### Module removability

With PlayerBot disabled or absent, Chipa must still build and run normally.

## 2. Runtime structure and lifecycle

All bots use a real Core `Player` object. No FakePlayer/NpcPlayer abstraction is introduced.

Bot types:

- Human
- SelfBot
- ManagedBot
- RandomBot
- future AltBot profiles

### SelfBot

SelfBot keeps the real client, real `WorldSession` and real `Player`, then attaches PlayerBot AI. Initial control policy is human movement + AI combat casting.

### Managed/Random Bot

Managed bots use a socketless `WorldSession`, a real `Player`, and the same PlayerbotAI/class rotation layer used by SelfBot.

### Lifecycle

```text
UNLOADED
→ LOGIN_PENDING
→ LOADING
→ IN_WORLD
→ AI_READY
→ ACTIVE
→ LOGOUT_PENDING
→ SAVING
→ REMOVED
```

Special states include TELEPORTING, DEAD, DISABLED and ERROR/QUARANTINED.

Bot sessions are manager-owned and must not be treated as ordinary network sessions unless explicitly required by Core semantics.

## 3. Core Bridge

### Module framework

The module framework must be generic, not PlayerBot-specific.

Two independent switches are required conceptually:

```text
MODULES=0/1
Playerbots.Enable=0/1
```

`MODULES` controls compilation. `Playerbots.Enable` controls runtime activation.

### Script tick

Reuse the existing PlayerScript update hook. Do not add a second Core AI tick.

### Socketless WorldSession

A bot session must tolerate `m_Socket == nullptr`. Existing null-safe paths are reused; unsafe socket dereferences must be guarded generically.

### Bot login

Do not synthesize a fake CMSG login packet and do not duplicate the Core login query holder in the module. Character loading remains Core-owned and is exposed through a small server-side bot-login bridge.

Bulk/random logins must be asynchronous and paced.

### Teleport

Far teleport should reuse the existing server-side worldport finalizer. Near teleport should expose/reuse an internal server-side finalization path instead of forging client ACK packets.

### Core modification budget

For every feature:

1. zero Core changes is best
2. Adapter-only changes are normal
3. one small generic Core helper is acceptable
4. repeated bot-specific Core changes trigger architecture review

## 4. Compatibility Adapter

The Adapter is a selective anti-corruption boundary, not a wrapper around the whole Core.

Stable types such as `Player*`, `Unit*`, `Group*`, `ObjectGuid`, `Position`, and `SpellInfo const*` may be used directly where appropriate.

Volatile surfaces are isolated behind compatibility contracts:

```text
compat/
├─ BotSessionCompat
├─ BotLoginCompat
├─ BotMovementCompat
├─ BotSpellCompat
├─ BotGroupCompat
├─ BotLfgCompat
├─ BotCommandCompat
├─ BotConfigCompat
├─ BotLocaleCompat
└─ BotDatabaseCompat
```

### Integration callbacks

Core callback naming differences are normalized in an integration layer before entering the PlayerBot engine.

### Ownership/thread rules

- Adapter code does not silently take ownership of Core objects.
- Raw `Player*` must not cross unsafe asynchronous DB/job boundaries.
- Async requests carry GUID/account/request/generation identifiers.
- Worker threads do not mutate `Player`, `Map`, Group, Spell, Movement or other live game objects.

### Spell identity

MoP numeric spell IDs are canonical for class/spec AI. Name-based lookup is compatibility-only and not used in the AI hot path.

### Config

Load configuration once into a validated runtime snapshot. AI code does not repeatedly query config files/managers on each tick.

### DB

Repository/DTO boundaries hide QueryResult/prepared-statement details from the AI engine.

## 5. PlayerBot AI Engine and 11 classes / 34 specializations

### One scheduler, one arbitration path

Do not run two independent combat schedulers. Official PlayerBots provides the long-term Strategy/Trigger/Action/Value model; DigiD702 supplies the practical MoP execution/rotation donor.

```text
PlayerbotMgr
  ↓
PlayerbotAI
  ↓
Bot State / Context
  ↓
Strategy Engine
  ├─ Values
  ├─ Triggers
  ├─ Actions
  ├─ Multipliers
  └─ Queue / Arbiter
  ↓
Functional Services
  ↓
Class / Spec Provider
  ↓
Spell Service
  ↓
Compat / Chipa Core
```

### Strategy

Strategies define behavior and preference, not direct execution. Structural, Behaviour, Feature and Context strategies are kept distinct.

### Trigger

Triggers observe conditions and must be side-effect free.

### Action

Actions execute one meaningful unit of behavior after usefulness/possibility validation.

### Values/cache

Repeated state calculations use named Values and HOT/WARM/COLD caching. No DB I/O is allowed in AI hot paths.

### Roles

Role is derived primarily from active specialization, not class alone.

Primary roles:

- Tank
- Healer
- Melee DPS
- Ranged DPS

Secondary capabilities such as interrupt, CC, off-heal, dispel and defensive are modeled separately.

### MoP specialization coverage

The architecture tracks all 11 classes / 34 specializations independently:

- Warrior: Arms, Fury, Protection
- Paladin: Holy, Protection, Retribution
- Hunter: Beast Mastery, Marksmanship, Survival
- Rogue: Assassination, Combat, Subtlety
- Priest: Discipline, Holy, Shadow
- Death Knight: Blood, Frost, Unholy
- Shaman: Elemental, Enhancement, Restoration
- Mage: Arcane, Fire, Frost
- Warlock: Affliction, Demonology, Destruction
- Monk: Brewmaster, Mistweaver, Windwalker
- Druid: Balance, Feral, Guardian, Restoration

### Spec registry

Concrete spec implementations register through a factory/registry. The generic engine does not include concrete class headers directly.

### Generic combat services

Reusable services include:

- target selection
- threat
- interrupt
- dispel
- heal target selection
- AoE safety
- crowd control
- facing/range/LoS
- cooldown/burst
- pet/minion behavior

Class/spec code provides capabilities and concrete spell choices rather than reimplementing generic policy.

### Monk as reference class

Monk is the initial architecture reference:

- Windwalker → DPS path
- Brewmaster → Tank/threat/mitigation path
- Mistweaver → Healing/triage/group-value path

The first POC target is SelfBot + Windwalker.

## 6. DB / Config / Data architecture

### Logical ownership

```text
AUTH       → real Accounts
CHARACTERS → real Characters and character state
WORLD      → real world/game data
PLAYERBOTS → bot-specific persistent/reference/cache/generated data
```

Actual Account and Character records are never duplicated in PlayerBot-specific tables.

### Data classes

- D0 Core-owned data
- D1 PlayerBot persistent data
- D2 PlayerBot reference data
- D3 rebuildable cache
- D4 generated dataset
- D5 runtime-only state

Runtime AI state such as current target, action queue and trigger values must not be persisted each tick.

### PlayerBot DB

A separate logical PlayerBot DB is the long-term target for:

- account/character registries
- preferences and custom strategies
- RandomBot state
- speech
- travel graph
- item/equipment caches
- weight data
- dataset manifest
- schema version

SelfBot POC must remain possible without a dedicated PlayerBot DB.

### Dataset provenance

Every imported/generated dataset records source, expansion/build, version, hash/generator and verification state.

Legacy data is not assumed MoP-compatible because it appears in a MoP repository.

### Travel data

Historical travel datasets require explicit MoP 5.4.8 validation or regeneration.

### Config/DB/runtime separation

- Config = server-wide operator policy
- DB = persistent bot state and datasets
- Runtime memory = what a bot is doing now

## 7. Advanced systems, performance, concurrency and fault isolation

### Functional services

Quest, Loot, Vendor, Travel, LFG, Dungeon, Raid, PvP and RandomBot are separate services/capabilities.

### Quest

Quest planning is separated into candidate selection, eligibility, objective planning, travel/interaction, completion and reward. Core quest state remains the source of truth.

### Travel

Travel is hierarchical:

```text
High-level travel graph
→ next travel segment
→ local navigation/pathfinding
→ MotionMaster/Core movement
```

Travel planning is queued and budgeted. Thread safety must be proven before live Core path/nav objects are used off-thread.

### Group/Dungeon/Raid

Group/Raid AI uses shared derived contexts such as target, tank/healer assignments, encounter state and cooldown coordination. These contexts do not replace Core Group/Raid state.

Dungeon/Raid/PvP behavior overlays the base class/spec AI instead of duplicating rotations.

### RandomBot

RandomBot separates population management from AI scheduling.

Foreground and background bots receive different scheduling priorities.

Priority principle:

1. Human/Worldserver stability
2. SelfBot/human-group bot responsiveness
3. active/nearby RandomBots
4. background RandomBot fidelity

### Scheduler

AI work is budgeted per world update and desynchronized with due-time offsets/jitter. Event-driven handling is preferred where available.

### Performance gates

L100/L250/L500/L1000 are test stages, not guaranteed supported bot counts.

- L100 → multi-bot functional stability
- L250 → scheduler/cache/login pacing
- L500 → load shedding/background quality reduction
- L1000 → stress/scalability boundary observation

### Load shedding

Under pressure, reduce nonessential background work first, then suspend new RandomBot activities/logins, then trim idle/background bots. Human and SelfBot/managed-human-group paths are protected first.

### Fault isolation

Fault scopes:

- Action fault
- Bot fault
- Feature fault
- Dataset fault
- Module fault
- Core fault

Repeated bot-local fatal behavior may quarantine that bot. Optional feature failures degrade/disable that feature instead of stopping all PlayerBots where possible.

## 8. Validation, POC gate, upstream and release architecture

### Evidence-based verification

Implementation state and verification state are separate.

Verification lifecycle:

```text
V0 NOT_PRESENT
V1 SOURCE_PRESENT
V2 PORTED
V3 STATIC_PASS
V4 BUILD_PASS
V5 BOOT_PASS
V6 RUNTIME_PASS
V7 GAME_PASS
V8 REGRESSION_PASS
V9 RELEASE_VERIFIED
```

Only evidence-backed features may be declared supported.

### Initial POC gates

```text
POC-G0 Baseline Core regression record
POC-G1 Generic Module Infrastructure
POC-G2 PlayerScript OnUpdate bridge
POC-G3 SelfBot attach/detach
POC-G4 SelfBot control ownership
POC-G5 Windwalker vertical combat path
POC-G6 Human regression
POC-G7 PlayerBot disabled/module absent regression
```

The initial POC intentionally excludes PlayerBot DB, RandomBot, Travel, full Quest AI, LFG, Raid, BG and the full 34-spec rollout.

### ManagedBot gates

After SelfBot architecture viability is proven:

- socketless session
- server-side character login
- teleport finalization
- repeated logout/lifecycle
- human + bot group behavior

### Verification vs quality

A specialization can pass verification while still having only basic AI quality. Maturity/quality grades are therefore tracked separately from V0~V9.

### Regression modes

- module absent
- module compiled but PlayerBot disabled
- PlayerBot active

Human Core behavior must remain correct in all three modes.

### Failure injection

Required scenarios include bad config, unavailable PlayerBot DB, missing dataset, login failure, teleport failure, repeated action/path failure, stale async result, bot logout during async completion, feature circuit breaker and shutdown with pending work.

### Upstream policy

No automatic upstream merge. Changes from DigiD702, official PlayerBots, Core references or PR #389 are reviewed feature-by-feature, classified for expansion dependence, ported through Chipa compatibility boundaries, and reverified.

### Provenance

External code records repository, branch, commit SHA, original path, destination, adaptation reason and verification state. Dataset provenance is tracked separately.

### Architecture freeze

Architecture v0.1 freezes responsibility boundaries and validation rules, not implementation details such as exact scheduler algorithm, tick milliseconds, per-bot memory budgets or final supported bot counts.

## Architecture Decision summary

The full design process produced **ADR-001 through ADR-180**. The most important invariant decisions are:

1. Chipa runtime Core remains the final source of truth.
2. PlayerBot is a removable module.
3. Core changes are generic and minimal.
4. Compatibility isolates external Core/API differences.
5. SelfBot and ManagedBot share AI/rotation.
6. All bots use a real Core Player.
7. MoP spell IDs are canonical in class/spec AI.
8. 11 classes / 34 specs are independently tracked.
9. Generic combat/role policy is separated from concrete spell choice.
10. WotLK code/data is not accepted without MoP validation.
11. DB/config/runtime/datasets have separate ownership.
12. Human/Worldserver stability outranks background RandomBot fidelity.
13. Feature presence is not feature completion.
14. Source and dataset provenance are mandatory.
15. Architecture v0.1 is the implementation baseline.

## Open architecture items

There are **AO-001 through AO-050** unresolved implementation/measurement decisions. These are intentionally not guessed closed. They are resolved with source analysis, POC evidence, benchmark data, game tests or new superseding ADRs.

See [`KNOWN_ISSUES.md`](KNOWN_ISSUES.md) for the open-item index.

## Next phase

Architecture is complete. The next artifact is `POC_SPEC.md`, followed by:

```text
POC implementation
→ POC result
→ PORTING_PLAN.md
→ IMPLEMENTATION_PLAN.md
→ SCHEDULE.md
→ feature-by-feature development
```
