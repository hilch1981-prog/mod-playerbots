# Chipa PlayerBot V2 — POC Specification v0.1

Target: World of Warcraft: Mists of Pandaria 5.4.8 (Build 18414)

Status: **READY FOR IMPLEMENTATION**

Primary project: `hilch1981-prog/mod-playerbots` / `mop-5.4.8-v2`

Runtime integration target: `hilch1981-prog/MOP_V2_Repack` / `playerbot-v2-poc`

Architecture baseline: `docs/playerbots/ARCHITECTURE.md`

---

## 1. POC objective

The first POC does **not** attempt to port the complete PlayerBot system.

Its purpose is to prove one vertical slice:

```text
Chipa Player
  -> PlayerScript OnUpdate
  -> PlayerBot module
  -> SelfBot attach
  -> PlayerbotAI
  -> Windwalker rotation decision
  -> Spell service / compatibility layer
  -> normal Chipa Core spell cast
```

If this path works without breaking normal human-player behavior, Architecture v0.1 is considered viable for implementation planning.

---

## 2. POC success statement

The POC is successful only when all of the following are true:

1. The PlayerBot module can be built as an optional module.
2. `MODULES=0` still builds and boots the original Chipa runtime path.
3. A normal logged-in human character is not affected unless SelfBot is explicitly enabled.
4. A Monk/Windwalker human character can attach PlayerBot AI to itself without replacing the real client session.
5. User movement remains under user control.
6. The AI can select and cast a valid MoP Windwalker combat spell through normal Core spell mechanics.
7. SelfBot can be detached cleanly.
8. Login, logout, death, teleport/map transition and re-login do not leave a stale AI registration.
9. No PlayerBot class/spec policy is moved into Chipa Core.
10. The feature can be disabled/removed without preventing normal server operation.

---

## 3. Explicitly out of scope

The first POC must not expand into these areas:

- RandomBot population
- socketless ManagedBot login
- separate PlayerBot database
- full Quest AI
- Loot/Vendor automation
- Travel graph
- LFG/LFR
- Dungeon encounter AI
- Battleground/Arena
- Raid AI
- Pet AI completion
- all 34 specializations
- production performance tuning
- 100/250/500/1000 Bot load testing

These belong to later gates after the SelfBot vertical slice is proven.

---

## 4. Frozen source baselines

### Runtime Core

- Repository: `hilch1981-prog/MOP_V2_Repack`
- Branch: `repack-main`
- Frozen baseline: `0739d072f8f1f42523f04cca4b2607d88a01def4`
- Integration branch: `playerbot-v2-poc`

### Primary MoP donor

- Repository: `DigiD702/mod-playerbots`
- Branch: `main`
- Frozen baseline: `13bc0ffa93c6b6625ed28fe2a03e0c071215ff48`

### Generic PlayerBot reference

- Repository: `mod-playerbots/mod-playerbots`
- Branch: `master`
- Use: Strategy/Trigger/Action/Value and generic feature design reference only.

### Historical MoP reference

- Legends of Azeroth PR #389
- Use: historical port/core-hook/AI comparison only.

All imported donor code must be recorded in the future `PORTING_MATRIX.md` with source repository, branch, commit, original path, destination and adaptation notes.

---

## 5. Repository workflow

### Main development repository

All PlayerBot V2 module code and documentation are developed in:

```text
hilch1981-prog/mod-playerbots
branch: mop-5.4.8-v2
```

### Runtime integration repository

Chipa Core build/integration changes are made in:

```text
hilch1981-prog/MOP_V2_Repack
branch: playerbot-v2-poc
```

### Bootstrap submodule note

The current `MOP_V2_Repack/playerbot-v2-poc` bootstrap initially pins the DigiD702 donor commit for reproducibility.

Once the minimum Chipa-compatible POC module exists in `hilch1981-prog/mod-playerbots/mop-5.4.8-v2`, the runtime integration branch must repoint `modules/mod-playerbots` to this repository and pin the exact POC commit SHA.

Do not track a moving branch without a pinned gitlink commit.

---

## 6. POC implementation phases

### POC-G0 — Baseline verification

Before modifying runtime behavior:

- build current `MOP_V2_Repack/repack-main`
- boot worldserver
- human login/logout
- movement
- basic combat
- teleport/map transition

Record this as the regression baseline.

No PlayerBot result is meaningful without this baseline.

---

### POC-G1 — Generic module infrastructure

Goal: provide a generic optional module loading path in Chipa Core.

Expected existing Core/build files to review or modify:

```text
/CMakeLists.txt
/cmake/options.cmake
/src/server/worldserver/CMakeLists.txt
/src/server/scripts/ScriptLoader.cpp
```

Expected new generic infrastructure:

```text
/modules/CMakeLists.txt
/modules/ModulesLoader.cpp.in
```

Exact paths may be adjusted to the Chipa tree during implementation, but the responsibility boundary is fixed.

Requirements:

- generic `MODULES` compile switch
- no PlayerBot-specific code in generic Module Loader
- generated/aggregate `AddModulesScripts()` entry point
- Core game library must not include PlayerBot headers
- `MODULES=0` remains valid

PASS:

- build succeeds with modules disabled
- build succeeds with modules enabled
- a minimal module registration callback is executed on worldserver startup

FAIL:

- Core requires PlayerBot classes to compile
- Module Loader contains class/spec/bot policy
- disabling modules breaks the original Core build

---

### POC-G2 — Minimal PlayerBot integration scripts

Goal: translate Chipa script hooks into module-owned PlayerBot events.

Proposed module paths:

```text
src/integration/PlayerbotPlayerScript.cpp
src/integration/PlayerbotWorldScript.cpp
src/integration/PlayerbotCommandScript.cpp
```

Required Chipa hooks already expected by architecture:

- `PlayerScript::OnUpdate(Player*, uint32 diff)`
- login/logout hooks as needed
- chat/command registration
- `WorldScript::OnConfigLoad` or equivalent for module config

Important donor difference:

Digi uses hook/API naming that is not guaranteed to match Chipa exactly. Hook names are adapted in `integration/`; Chipa Core is not renamed to imitate donor APIs.

PASS:

```text
PlayerScript OnUpdate
  -> module integration
  -> PlayerbotMgr lookup
  -> normal human not registered as SelfBot -> immediate return
```

---

### POC-G3 — Minimal compatibility layer

Only the adapters necessary for the first SelfBot POC are implemented.

Proposed scope:

```text
src/compat/BotCompatCommon.h
src/compat/BotCapabilities.h
src/compat/BotCommandCompat.*
src/compat/BotConfigCompat.*
src/compat/BotSpellCompat.*
```

Not required for POC-G3:

```text
BotSessionCompat
BotLoginCompat
BotLfgCompat
PlayerBot database adapter
```

Those belong to ManagedBot/advanced phases.

Rules:

- Adapter isolates API differences, not the entire Core.
- Stable Core types such as `Player*`/`Unit*` may pass through directly where safe.
- Adapter must not contain Windwalker priority policy.
- command security maps to Chipa `AccountTypes`; do not port AzerothCore RBAC into Chipa Core.
- configuration uses existing Chipa ConfigMgr capabilities; do not invent a new Core config system.

---

### POC-G4 — PlayerbotMgr minimal registry

POC PlayerbotMgr responsibilities:

```text
SelfBot registry
attach/detach coordination
per-player PlayerbotAI ownership
AI update dispatch
safe logout cleanup
```

It must not own:

```text
Windwalker rotation
Quest logic
Travel logic
DB schema
LFG policy
```

Required behavior:

- duplicate attach is rejected or treated idempotently
- detach removes AI cleanly
- logout always unregisters SelfBot
- stale `Player*` is never retained after player destruction

---

### POC-G5 — SelfBot attach/detach

Primary donor concept: DigiD702 `.playerbots self` behavior.

POC control model:

```text
Real client session: retained
Player object: retained
Human movement: retained
AI combat casting: enabled
AI autonomous movement: disabled
Quest/Vendor/Travel automation: disabled
```

Suggested initial control policy name:

```text
CAST_ONLY
```

SelfBot is not a socketless bot and must not create a replacement `WorldSession`.

Test cases:

- attach once
- attach twice
- detach
- attach after detach
- logout while attached
- relog
- death/revive
- map transition
- target change by human
- manual movement while AI is active

---

### POC-G6 — Minimal AI engine

The POC uses one scheduler/arbitration path only.

Do not run an official AC Strategy engine and a Digi direct rotation as two independent combat loops.

Minimum path:

```text
PlayerbotAI
  -> state check
  -> target/context
  -> emergency/interrupt candidate
  -> Windwalker rotation fallback
  -> one selected action
  -> SpellService
```

Required state distinction:

```text
Disabled
Dead
NonCombat
Combat
```

Lifecycle state remains separate from AI state.

The first POC may use a deliberately thin Strategy/Action model. Full Value/Trigger/Multiplier expansion comes after architecture viability is demonstrated.

---

### POC-G7 — Monk Windwalker reference implementation

Primary MoP donor file to inspect:

```text
DigiD702/mod-playerbots
src/rotations/BotRotation_Monk.cpp
```

Canonical spell identity is the MoP numeric Spell ID, not localized spell text.

The POC should validate at least the known donor priority concepts:

```text
Stance of the Fierce Tiger
Rising Sun Kick
Tiger Palm / Tiger Power
Fists of Fury
Blackout Kick
Jab
```

The exact production priority is not the POC target. The goal is to prove the full execution chain.

Minimum context:

- specialization = Windwalker
- valid hostile target
- target alive
- bot alive
- resource check (Energy/Chi)
- known spell check
- cooldown/GCD check
- range/LoS/facing according to Chipa spell mechanics
- current cast/channel protection

The AI must use normal Core cast behavior. It must not directly force damage, aura, cooldown or resource changes.

---

## 7. Human control and conflict rules

Default SelfBot POC ownership:

| Capability | Owner |
|---|---|
| Movement | Human |
| Jump/positioning | Human |
| Primary selected target | Human |
| Combat spell selection | AI |
| Interrupt/defensive | AI, within POC scope |
| Quest navigation | Disabled |
| Vendor/loot automation | Disabled |

If the user moves, AI movement must not fight the client.

A later POC may add a short manual-cast suppression window based on `OnSpellCast`, but that is not required to prove the first vertical slice unless immediate AI/client cast conflict makes it necessary.

---

## 8. Core modification budget for the first POC

### Expected generic Core/build changes

Approximately four existing build/registration files may change for module support:

```text
root CMakeLists.txt
cmake/options.cmake
worldserver CMakeLists.txt
ScriptLoader.cpp
```

New generic module-loader files are allowed.

### Expected Core gameplay changes

For the initial SelfBot POC:

```text
WorldSession bot-login bridge: NOT REQUIRED
socketless session changes: NOT REQUIRED
near teleport bot finalizer: NOT REQUIRED
LFG helpers: NOT REQUIRED
Group target-icon getter: NOT REQUIRED
```

If implementation unexpectedly requires gameplay-Core changes for SelfBot, stop and document the reason before expanding the diff.

---

## 9. Files/functions explicitly prohibited in the first POC

Do not add:

- Monk/Windwalker logic to `Player.cpp`
- PlayerBot strategy decisions to `WorldSession`
- PlayerBot-specific group APIs such as `GetBotTank()`
- RandomBot policy to Character login handlers
- direct WotLK opcode/spell/talent compatibility hacks in Core
- a second SelfBot-only rotation system
- direct character/quest DB mutation from AI
- fake client packets merely to imitate an ACK when a server-side path can be designed later

---

## 10. Donor import rules

DigiD702 is the primary MoP donor, but files are not copied blindly.

For each imported component:

1. identify donor file and commit
2. classify code as generic, MoP-specific, Core-dependent or unnecessary
3. adapt external API calls through `compat/` where needed
4. preserve applicable license/copyright headers
5. remove or rewrite AC/SkyFire assumptions that do not match Chipa
6. keep MoP spell IDs only after verifying they apply to Build 18414 behavior
7. record the result in `PORTING_MATRIX.md` after the POC proves viable

PR #389 is reference-only unless an individual implementation is explicitly selected and documented.

---

## 11. Build matrix

At minimum test:

### B0 — Chipa baseline

```text
MODULES absent/original configuration
PlayerBot code absent
```

Expected: PASS.

### B1 — Modules disabled

```text
MODULES=0
```

Expected: same functional Core behavior as baseline.

### B2 — Modules enabled, PlayerBot disabled

```text
MODULES=1
Playerbots.Enable=0
```

Expected: build/boot PASS; normal human behavior unchanged.

### B3 — PlayerBot enabled

```text
MODULES=1
Playerbots.Enable=1
```

Expected: SelfBot feature available; normal humans unaffected until explicitly attached.

---

## 12. Game test matrix

### PB-SELF-001 — Attach

Precondition: human Monk/Windwalker logged in.

Expected: AI attaches once; real session remains active.

### PB-SELF-002 — Duplicate attach

Expected: no duplicate AI object/update registration.

### PB-SELF-003 — Movement ownership

Move character manually while AI is active.

Expected: AI does not call autonomous chase/follow movement in CAST_ONLY mode.

### PB-WW-001 — Combat entry

Select valid hostile target and enter combat.

Expected: AI reaches combat decision path.

### PB-WW-002 — Valid spell cast

Expected: at least one Windwalker priority spell is selected and cast through normal Core spell mechanics.

### PB-WW-003 — Resource starvation

Expected: AI does not repeatedly attempt an impossible spender when Energy/Chi requirements are not met.

### PB-WW-004 — Range/LoS

Expected: no illegal forced cast; failure is contained to the action.

### PB-WW-005 — Channel/cast protection

Expected: Fists of Fury or another active cast/channel is not immediately replaced by normal rotation spam.

### PB-SELF-004 — Death/revive

Expected: AI does not execute combat actions while dead and recovers after revive.

### PB-SELF-005 — Detach

Expected: all SelfBot AI updates stop immediately and the human remains playable.

### PB-SELF-006 — Logout/relog

Expected: no stale SelfBot entry, dangling AI or duplicate attach after relog.

### PB-REG-001 — Normal human regression

Use a non-SelfBot human character.

Expected: login, movement, combat, group and logout remain unchanged.

### PB-REG-002 — PlayerBot disabled

Expected: worldserver behaves normally with `Playerbots.Enable=0`.

### PB-REG-003 — Modules disabled

Expected: original Chipa build/runtime path remains valid with `MODULES=0`.

---

## 13. Verification states required to close the POC

The POC feature must advance through:

```text
V1 SOURCE_PRESENT
V2 PORTED
V3 STATIC_PASS
V4 BUILD_PASS
V5 BOOT_PASS
V6 RUNTIME_PASS
V7 GAME_PASS
V8 REGRESSION_PASS
```

`V9 RELEASE_VERIFIED` is not required for the initial POC.

The POC must not be called complete at `SOURCE_PRESENT` or `BUILD_PASS` only.

---

## 14. Evidence required

For the POC closeout, preserve:

- exact Chipa Core commit
- exact module commit
- build configuration
- build result
- worldserver startup result
- SelfBot attach/detach server log
- Windwalker cast evidence (server/game/combat log or video/screenshot)
- human regression result
- known failures/limitations

The evidence should later be summarized in a POC result document or GitHub issue/PR.

---

## 15. Stop conditions

Stop implementation and open an architecture review if any of these occur:

- SelfBot requires replacing the real human WorldSession
- PlayerbotAI must be included from Core gameplay code
- Windwalker/class policy must be added to Core to compile
- the module requires wholesale Digi Core replacement
- a large AzerothCore compatibility layer begins spreading through Core
- normal human Player behavior changes when SelfBot is not enabled
- the proposed Module Loader cannot remain generic

A stop condition is not a failed project; it is evidence that an ADR/AO must be revisited.

---

## 16. Rollback

The POC must remain reversible.

### Module rollback

Remove/disable PlayerBot module or set:

```text
Playerbots.Enable=0
```

### Build rollback

Build with:

```text
MODULES=0
```

### Git rollback

Runtime Core POC work remains isolated on:

```text
MOP_V2_Repack/playerbot-v2-poc
```

and must not be merged into `repack-main` until the relevant gates pass.

No PlayerBot DB migration is part of the first POC, so database rollback is intentionally avoided in this phase.

---

## 17. POC completion decision

### PASS

All of the following are true:

- generic optional module infrastructure works
- SelfBot attach/detach works
- real human movement/session remains authoritative
- Windwalker reaches a valid normal Core cast
- no stale AI lifecycle issue is observed in the defined tests
- normal human regression tests pass
- `MODULES=0` and `Playerbots.Enable=0` remain safe
- no architecture stop condition is triggered

### PARTIAL PASS

The architecture chain works, but a contained class/rotation/compat issue remains. The issue must be documented before deciding whether implementation planning can proceed.

### FAIL / ARCHITECTURE REVIEW

A fundamental boundary fails, especially if the design requires broad PlayerBot-specific Core changes or breaks normal human behavior.

---

## 18. After POC PASS

Only after this document's POC is proven should the project create the next planning documents:

```text
PORTING_PLAN.md
  -> exact feature-by-feature donor migration plan

IMPLEMENTATION_PLAN.md
  -> PR/file/dependency/test execution plan

SCHEDULE.md
  -> development sequence and estimates based on real POC evidence
```

The next functional expansion after POC should then proceed through ManagedBot session/login/teleport gates before large RandomBot or advanced world-AI work.
