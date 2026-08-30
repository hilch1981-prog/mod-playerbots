# Chipa PlayerBot V2 Source Policy

Target: WoW Mists of Pandaria 5.4.8 Build 18414

## Source hierarchy

### S0 — Chipa runtime source of truth

- `hilch1981-prog/MOP_V2_Repack`
- Final runtime target, build target and regression environment.
- External projects adapt to this Core.

### S1 — Genealogy/reference

- `alexkulya/pandaria_5.4.8`
- Used for lineage/reference only.

### S2 — MoP Core-hook reference

- `DigiD702/skyfire_548_playerbots`
- First reference for socketless session, server-side bot login, teleport finalization and other MoP Core bridge requirements.

### S3 — Primary MoP PlayerBot implementation donor

- `DigiD702/mod-playerbots`
- Primary donor for SelfBot, MoP rotations, Monk, class/spec implementation and practical MoP AI behavior.

### S4 — Generic PlayerBot feature donor

- `mod-playerbots/mod-playerbots`
- Primary donor/reference for Strategy, Trigger, Action, Value, world AI, dungeon/raid patterns, performance and mature PlayerBot features.
- WotLK-specific code/data must be adapted or rejected.

### S5 — Official PlayerBot Core requirement reference

- `mod-playerbots/azerothcore-wotlk` Playerbot branch
- Used to identify Core capabilities expected by upstream PlayerBots.
- Never copied wholesale into Chipa Core.

### S6 — Historical MoP port evidence

- Legends of Azeroth PR #389
- Historical porting evidence and comparison source.
- Not the runtime base and not automatically trusted as MoP-validated data.

## Decision rule

- What must final code run against? → **Chipa S0**
- How should a feature be implemented for MoP 5.4.8? → **Digi first**
- What should PlayerBot be capable of long-term? → **official mod-playerbots**
- What historical MoP solutions/data can be compared? → **PR #389**

## Project source categories

- **P0 Chipa Native Source** — original project/Core code
- **P1 Ported PlayerBot Source** — imported/adapted PlayerBot implementation
- **P2 Compatibility/Adapter Source** — Chipa/external API translation
- **P3 Data/Config** — configuration and SQL/data assets
- **P4 Reference Source** — source links/SHAs/documentation only
- **P5 Test/Validation** — test harnesses, matrices, logs and evidence
- **P6 Generated Source** — generated loader/data/cache artifacts

## Porting rules

1. Never wholesale cherry-pick or merge PR #389.
2. Never wholesale merge Digi Core into Chipa Core.
3. Never modify Chipa Core merely to imitate AzerothCore APIs when an Adapter can solve the difference.
4. Never accept WotLK spell/talent/opcode/data semantics without MoP 5.4.8 validation.
5. Class/spec AI stays outside Core.
6. SelfBot uses the same AI/rotation framework as ManagedBot; it does not receive a separate rotation implementation.
7. C++ and SQL changes remain separable and independently reviewable.
8. Large all-in-one PlayerBot PRs are prohibited; port feature-by-feature.
9. Source presence is not completion; verification evidence is required.

## Mandatory code provenance

For imported/adapted external code record:

- source repository
- source branch
- source commit SHA
- original path
- destination path
- port type/reason
- MoP adaptation notes
- verification state
- retained copyright/license headers

## Dataset provenance

Datasets require separate metadata:

- dataset name/version
- source repository/file or generator
- expansion/client build
- source commit/hash
- generator version where applicable
- content hash
- validation state

A dataset located in a MoP repository is not automatically a MoP-validated dataset.

## Upstream update policy

Upstream updates are reviewed and classified; they are never merged automatically.

Suggested classification:

- U0 documentation-only
- U1 expansion-independent bug fix
- U2 generic AI feature
- U3 Core API-dependent change
- U4 WotLK-specific behavior
- U5 DB/data change
- U6 performance/scheduler change
- U7 security/stability-critical change

Each selected change is re-ported through the current Chipa compatibility boundary and re-tested.
