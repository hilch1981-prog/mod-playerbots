# Chipa PlayerBot V2 — Source Baseline

Target: World of Warcraft: Mists of Pandaria 5.4.8 (Build 18414)

## Project Source of Truth

- Repository: `hilch1981-prog/mod-playerbots`
- Branch: `mop-5.4.8-v2`
- Role: **canonical PlayerBot V2 project hub**, module source, architecture/docs, project Issues, POC/feature tracking, validation tracking and releases.
- Governance: [`PROJECT_GOVERNANCE.md`](PROJECT_GOVERNANCE.md)

**Rule:** when deciding where a PlayerBot V2 Issue, feature, design note, POC status or module change belongs, use this repository unless an explicit governance/ADR change says otherwise.

## Runtime Core Source of Truth

- Repository: `hilch1981-prog/MOP_V2_Repack`
- Runtime baseline branch: `repack-main`
- Frozen baseline commit: `0739d072f8f1f42523f04cca4b2607d88a01def4`
- Integration branch: `playerbot-v2-poc`
- Role: final Chipa MoP 5.4.8 runtime Core API, compile/boot/game target and human-player regression environment.

**Important:** runtime Core Source of Truth does not mean PlayerBot project hub. Core PRs are integration artifacts and should be linked from the owning Issue in `hilch1981-prog/mod-playerbots`.

## Primary MoP implementation donor

- Repository: `DigiD702/mod-playerbots`
- Branch: `main`
- Frozen commit: `13bc0ffa93c6b6625ed28fe2a03e0c071215ff48`
- Role: SelfBot, MoP 5.4.8 rotations, Monk, AI engine behavior and other MoP-specific implementation patterns.

## Generic PlayerBot upstream

- Repository: `mod-playerbots/mod-playerbots`
- Branch: `master`
- Role: generic Strategy / Trigger / Action / Value architecture, world AI, dungeon/raid features and long-term feature reference.
- Rule: WotLK-specific code/data requires MoP validation or rewrite.

## MoP core bridge reference

- Repository: `DigiD702/skyfire_548_playerbots`
- Role: socketless WorldSession, server-side bot login, teleport finalization and other MoP Core bridge references.
- Rule: do not replace the Chipa runtime core with this repository.

## Historical MoP port reference

- Pull request: `Legends-of-Azeroth/Legends-of-Azeroth-Pandaria-5.4.8#389`
- Role: historical porting evidence, core-hook comparison, DB and AI reference.
- Rule: not a runtime baseline and not automatically considered MoP-validated data.

## Source hierarchy

1. Where is PlayerBot V2 developed, documented and tracked? → `hilch1981-prog/mod-playerbots / mop-5.4.8-v2`
2. What must final code run against? → `MOP_V2_Repack`
3. How should a feature be implemented for MoP 5.4.8? → DigiD702 first
4. What should PlayerBot be capable of? → official `mod-playerbots`
5. What historical MoP solutions can be compared? → PR #389

## Provenance rule

Every imported external implementation should record:

- source repository
- branch
- commit SHA
- original path
- destination path
- reason for porting
- MoP adaptation notes
- verification state
- retained license/copyright headers
