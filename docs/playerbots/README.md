# Chipa PlayerBot V2 Documentation

Target: World of Warcraft: Mists of Pandaria 5.4.8 (Build 18414)

This directory is the documentation entry point for the `mop-5.4.8-v2` development branch.

## Architecture status

- Architecture v0.1: **complete (8/8 parts)**
- Architecture decisions: **ADR-001 ~ ADR-180**
- Open architecture items: **AO-001 ~ AO-050**
- Current phase: **architecture baseline frozen; POC specification next**

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

## Repository roles

- **Main PlayerBot V2 project hub:** `hilch1981-prog/mod-playerbots` / `mop-5.4.8-v2`
- **Runtime/integration Core:** `hilch1981-prog/MOP_V2_Repack` / `playerbot-v2-poc`
- **Primary MoP implementation donor:** `DigiD702/mod-playerbots`
- **Generic PlayerBot AI/feature donor:** `mod-playerbots/mod-playerbots`
- **MoP Core bridge reference:** `DigiD702/skyfire_548_playerbots`
- **Historical MoP port reference:** Legends of Azeroth PR #389

## Next implementation sequence

1. Freeze source baseline
2. Write `POC_SPEC.md`
3. Generic Module Loader
4. PlayerScript `OnUpdate` bridge
5. SelfBot attach/detach
6. Monk Windwalker minimal rotation
7. Human-player regression
8. If POC passes: `PORTING_PLAN.md` → `IMPLEMENTATION_PLAN.md` → `SCHEDULE.md`

## Non-negotiable rules

- `MOP_V2_Repack` remains the final runtime Core source of truth.
- PlayerBot AI and class/spec logic must not move into Core.
- Core changes are limited to generic infrastructure bridges.
- External projects are donors/references; no wholesale PR #389/Digi/AzerothCore merge.
- WotLK spell IDs, talents, opcodes and datasets require MoP 5.4.8 validation or rewrite.
- Source presence is not completion; evidence-based verification is required.
