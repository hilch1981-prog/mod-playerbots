# Chipa PlayerBot V2 Documentation

Target: World of Warcraft: Mists of Pandaria 5.4.8 (Build 18414)

This directory is the documentation entry point for the `mop-5.4.8-v2` development branch.

## Architecture status

- Architecture v0.1: **complete (8/8 parts)**
- Architecture decisions: **ADR-001 ~ ADR-180**
- Open architecture items: **AO-001 ~ AO-050**
- POC specification: **complete / ready for implementation**
- Current phase: **POC implementation preparation**
- First POC target: **SelfBot + Monk Windwalker**

## Documents

| Document | Purpose |
|---|---|
| [`ARCHITECTURE.md`](ARCHITECTURE.md) | Architecture v0.1 baseline: Core/Module boundary, lifecycle, compatibility, AI, DB, advanced systems, validation and release model |
| [`SOURCE_POLICY.md`](SOURCE_POLICY.md) | Rules for using Chipa, DigiD702, official PlayerBots and LoA PR #389 |
| [`SOURCE_BASELINE.md`](SOURCE_BASELINE.md) | Pinned repositories, branches and commit SHAs |
| [`CORE_HOOKS.md`](CORE_HOOKS.md) | Allowed/expected Chipa Core bridge changes and Core modification budget |
| [`API_COMPAT.md`](API_COMPAT.md) | Compatibility Adapter contracts between external PlayerBot code and Chipa Core |
| [`DATABASE.md`](DATABASE.md) | DB, Config, dataset, cache, migration and provenance architecture |
| [`TEST_MATRIX.md`](TEST_MATRIX.md) | V0~V9 verification model, POC gates, regression and performance gates |
| [`KNOWN_ISSUES.md`](KNOWN_ISSUES.md) | Architecture open issues and currently unverified areas |
| [`ADR_INDEX.md`](ADR_INDEX.md) | ADR-001 ~ ADR-180 architecture decision index |
| [`POC_SPEC.md`](POC_SPEC.md) | Executable first POC scope: generic module loader, SelfBot attach/detach, Windwalker vertical slice, PASS/FAIL and rollback |

## Repository roles

- **Main PlayerBot V2 project hub:** `hilch1981-prog/mod-playerbots` / `mop-5.4.8-v2`
- **Runtime/integration Core:** `hilch1981-prog/MOP_V2_Repack` / `playerbot-v2-poc`
- **Primary MoP implementation donor:** `DigiD702/mod-playerbots`
- **Generic PlayerBot AI/feature donor:** `mod-playerbots/mod-playerbots`
- **MoP Core bridge reference:** `DigiD702/skyfire_548_playerbots`
- **Historical MoP port reference:** Legends of Azeroth PR #389

## Implementation sequence

1. Source baseline frozen ✅
2. Architecture v0.1 frozen ✅
3. `POC_SPEC.md` complete ✅
4. Generic Module Loader
5. PlayerScript `OnUpdate` bridge
6. SelfBot attach/detach
7. Monk Windwalker minimal rotation
8. Human-player regression
9. If POC passes: `PORTING_PLAN.md` → `IMPLEMENTATION_PLAN.md` → `SCHEDULE.md`

## First POC boundaries

Included:

- generic optional module infrastructure
- minimal integration and compatibility layer
- SelfBot CAST_ONLY control model
- Monk Windwalker minimal combat vertical slice
- normal-human regression and module-disable validation

Explicitly excluded from the first POC:

- RandomBot
- socketless ManagedBot login
- PlayerBot DB
- Travel
- full Quest AI
- LFG/LFR
- Dungeon/Raid/PvP rollout
- all 34 specializations
- large-scale performance testing

## Non-negotiable rules

- `MOP_V2_Repack` remains the final runtime Core source of truth.
- PlayerBot AI and class/spec logic must not move into Core.
- Core changes are limited to generic infrastructure bridges.
- External projects are donors/references; no wholesale PR #389/Digi/AzerothCore merge.
- WotLK spell IDs, talents, opcodes and datasets require MoP 5.4.8 validation or rewrite.
- Source presence is not completion; evidence-based verification is required.
