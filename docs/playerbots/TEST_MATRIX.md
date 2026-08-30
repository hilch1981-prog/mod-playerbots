# Chipa PlayerBot V2 Verification / Test Matrix

Target: WoW Mists of Pandaria 5.4.8 Build 18414

## Verification lifecycle

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

`SOURCE_PRESENT` is not completion. `SUPPORTED` is reserved for evidence-backed release-verified functionality.

## Failure states

- COMPILE_FAIL
- BOOT_FAIL
- FUNCTION_FAIL
- REGRESSION_FAIL
- DATA_FAIL
- PERFORMANCE_FAIL
- CRASH
- BLOCKED

Use BLOCKED when a prerequisite is missing instead of mislabeling the implementation as a functional failure.

## Test case format

Every durable test case should record:

- Test ID
- feature/component
- build/commit
- preconditions
- configuration
- character/map/location
- steps
- expected result
- actual result
- status
- evidence
- known issue
- tester/date

Suggested IDs:

- `PB-BUILD-*`
- `PB-CORE-*`
- `PB-SELF-*`
- `PB-LOGIN-*`
- `PB-MONK-WW-*`
- `PB-TRAVEL-*`
- `PB-PERF-L100-*`
- `PB-REG-*`

## POC gates

### POC-G0 — Baseline

Before PlayerBot implementation, record current Chipa Core:

- build PASS
- worldserver boot PASS
- human login/logout PASS
- movement/combat sanity PASS

### POC-G1 — Generic Module Infrastructure

- `MODULES=0` keeps existing Core behavior
- `MODULES=1` builds generic module framework
- a minimal module script can register
- Core game libraries do not depend on PlayerBot headers

### POC-G2 — PlayerScript bridge

- existing PlayerScript update reaches PlayerBot manager
- non-bot human players take a fast no-op path
- no duplicate Core AI tick is introduced

### POC-G3 — SelfBot attach/detach

Validate:

- attach
- duplicate attach protection
- detach
- relog/logout
- death
- map change

### POC-G4 — SelfBot control ownership

Initial policy:

- human controls movement
- AI performs combat casting

Validate that AI does not forcibly follow/chase/flee in CAST_ONLY mode.

### POC-G5 — Windwalker vertical combat path

Validate:

- spec detection
- known spell checks
- target validity
- energy/Chi
- GCD/range/facing
- Rising Sun Kick
- Tiger Palm/Tiger Power
- Fists of Fury
- Blackout Kick
- Jab
- cooldown/buff/debuff handling

The goal is architecture viability, not final DPS tuning.

### POC-G6 — Human regression

With SelfBot not attached, human behavior remains normal:

- login/logout
- movement
- teleport
- combat
- group
- quest

### POC-G7 — Disable/remove regression

Validate separately:

- module compiled + `Playerbots.Enable=0`
- module absent / `MODULES=0` where supported

## ManagedBot gates

After SelfBot POC passes:

1. socketless WorldSession safety
2. server-side bot character login
3. near/far teleport completion
4. repeated login/logout lifecycle
5. human + managed bot group behavior

Repeated lifecycle testing must check for session/player/AI/registry/pending-request leaks.

## RandomBot gates

RandomBot begins only after ManagedBot lifecycle stability.

Validate:

- explicit account/character registry
- human account is never mistaken for module-owned bot account
- login pacing and pending limits
- gradual population ramp
- safe trim/logout under pressure

## 11 classes / 34 specs

Each specialization receives an independent verification state.

Minimum checks:

- spec detection
- role
- resource model
- single target
- AoE
- interrupt
- defensive
- cooldown
- buff/debuff
- movement compatibility
- death/revive
- group behavior
- SelfBot compatibility
- ManagedBot compatibility

### Tank additions

- threat
- taunt
- target retention
- mitigation
- pull/position/facing
- AoE threat
- emergency cooldown

### Healer additions

- heal target selection
- triage
- tank/group heal
- emergency heal
- dispel
- mana/resource
- resurrection
- overheal avoidance

### DPS additions

- ST/AoE priority
- burst
- target switching
- threat awareness
- movement

## AI quality grade

Verification and quality are separate.

Suggested quality scale:

- Q0 none
- Q1 basic rotation
- Q2 combat functional
- Q3 role functional
- Q4 group functional
- Q5 dungeon ready
- Q6 advanced/tuned

## Functional test groups

### Quest

- discover/eligibility/accept
- kill/loot/object objective
- progress/completion/reward
- chain/group quest
- failure/backoff/recovery

### Loot

- solo/party
- need/greed/pass
- full inventory
- protected/quest/upgrade/junk items

### Vendor

Test a mixed inventory and verify protected items are not sold.

### Travel

Test Engine and Dataset separately:

- node/link/route
- travel mode
- cancel/stale-generation result
- repath/stuck recovery
- Pandaria/expansion-specific dataset validity

### LFG/Dungeon

Separate queue/group formation from dungeon encounter AI.

### PvP

BG first; Arena later. Test queue, objectives, combat, death/resurrection and exit lifecycle.

### Raid/LFR

Allowed to remain NOT_PORTED/EXPERIMENTAL in early releases.

## Core regression modes

A. PlayerBot/module absent

B. module compiled but PlayerBot disabled

C. PlayerBot active

Human Core regression checks include:

- boot
- login/logout
- create/delete
- movement/teleport
- group/guild
- quest
- spell cast
- death/resurrection
- instance enter/exit
- LFG/BG as relevant
- shutdown

## DB / migration tests

- fresh install
- previous schema → current
- interrupted/failed migration handling
- retry/idempotence as designed
- dataset seed
- cache rebuild

Persistent data and rebuildable cache are tested separately.

## Failure injection

Required scenarios include:

- bad config
- PlayerBot DB offline
- required/optional dataset missing
- invalid bot login
- teleport failure
- repeated cast/action failure
- path failure/retry
- stale async result
- bot logout during async completion
- feature circuit breaker/quarantine
- server shutdown with pending jobs

## Performance gates

Bot counts are test stages, not automatic support claims.

### PERF-BASELINE

Record Core without PlayerBot:

- world update avg/p95
- CPU/memory
- DB latency where useful

### L100

Focus: multi-bot functional stability, queue stability, no runaway memory growth, human interaction.

### L250

Focus: scheduler backlog, cache effectiveness, login pacing, travel planning, population management.

### L500

Focus: load shedding, background frequency reduction, foreground/human responsiveness, path/grid budgets.

### L1000

Stress observation only unless later evidence promotes it to a support target.

Record at least:

- total/foreground/background bot distribution
- AI avg/p95/max
- world update avg/p95
- memory
- login queue
- path queue
- grid scans or equivalent spatial cost
- load-shedding state

## Evidence

Game PASS should retain useful evidence where possible:

- logs
- screenshot/video
- combat log
- DB query output
- performance metrics
- crash dump for failures

Architecture compliance is a merge gate even when the feature appears to work.
