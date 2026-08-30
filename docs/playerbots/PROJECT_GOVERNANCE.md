# Chipa PlayerBot V2 — Project Governance

Target: World of Warcraft: Mists of Pandaria 5.4.8 (Build 18414)

Status: **MANDATORY PROJECT OPERATING RULE**

This document exists to prevent repository-role confusion during development.

## 1. Canonical project hub

The canonical PlayerBot V2 project repository is:

```text
hilch1981-prog/mod-playerbots
branch: mop-5.4.8-v2
```

This repository is the **main home** for the project.

The following work belongs here by default:

- architecture and design documents
- POC specifications
- implementation plans
- porting matrix and source provenance
- PlayerBot module source code
- compatibility/adaptation source code that belongs to the module
- class/spec AI and rotations
- project Issues
- feature tracking
- validation tracking
- release notes and PlayerBot release artifacts
- developer-facing README and status

When there is any doubt about where a PlayerBot V2 task should be tracked, use this repository.

## 2. Runtime Core repository is NOT the project hub

Repository:

```text
hilch1981-prog/MOP_V2_Repack
```

Role:

- Chipa MoP 5.4.8 runtime Core Source of Truth
- compile/integration target
- worldserver runtime validation target
- human-player regression environment
- host for the minimum generic Core bridge changes required by PlayerBot V2

It is **not** the primary project-management repository for PlayerBot V2.

Do not move general PlayerBot planning, feature Issues, class/spec tracking or architecture governance into `MOP_V2_Repack` merely because a POC is compiled there.

## 3. Issue policy

### Create Issues in `hilch1981-prog/mod-playerbots`

Use this repository for:

- POC Gate tracking
- Build/Boot validation tracking
- SelfBot issues
- ManagedBot issues
- RandomBot issues
- class/spec status
- compatibility/adapter issues
- DB/dataset issues
- Travel/LFG/Dungeon/Raid/PvP design and validation
- performance gates
- architecture open issues
- upstream port requests

### Use `MOP_V2_Repack` PRs only when Core changes are required

A Core PR should be referenced from the corresponding PlayerBot Issue.

Example:

```text
mod-playerbots Issue: POC-G1 Generic Module Loader validation
        |
        +--> MOP_V2_Repack PR: minimum Core/module integration changes
```

The PlayerBot Issue remains the canonical tracking item.

## 4. Pull request policy

### `mod-playerbots`

Primary PR location for:

- PlayerBot module code
- compatibility layer
- AI engine
- rotations
- configuration owned by the module
- PlayerBot DB schema/data
- documentation
- tests owned by the PlayerBot project

### `MOP_V2_Repack`

Separate PR only for justified Core changes such as:

- generic module infrastructure
- socketless session safety
- server-side bot login bridge
- server-side teleport finalization
- optional generic Group/LFG read helpers

Every such Core PR must explain:

1. why the change cannot live in the module
2. why it cannot live in the compatibility adapter
3. the human-player regression impact
4. rollback behavior
5. the PlayerBot Issue that owns the work

## 5. Source of Truth distinction

Two different concepts must not be confused:

### Project Source of Truth

```text
hilch1981-prog/mod-playerbots / mop-5.4.8-v2
```

This is where PlayerBot V2 development is organized and maintained.

### Runtime Core Source of Truth

```text
hilch1981-prog/MOP_V2_Repack / repack-main
```

This is the final Core API/runtime that PlayerBot V2 must adapt to.

`MOP_V2_Repack` being the runtime Core Source of Truth does **not** make it the main PlayerBot project repository.

## 6. External donor policy

External projects remain donors/references:

- `DigiD702/mod-playerbots` — primary MoP implementation donor
- official `mod-playerbots/mod-playerbots` — generic AI/feature donor
- `DigiD702/skyfire_548_playerbots` — MoP Core bridge reference
- Legends of Azeroth PR #389 — historical MoP port evidence/reference

No external repository replaces the canonical project hub.

## 7. Current POC ownership

The first POC is owned and tracked from `hilch1981-prog/mod-playerbots`.

Current sequence:

```text
POC-G0 baseline
POC-G1 generic module infrastructure
POC-G2 PlayerScript OnUpdate integration
POC-G3 minimal compatibility layer
POC-G4 PlayerbotMgr registry
POC-G5 SelfBot attach/detach
POC-G6 minimal AI engine
POC-G7 Monk Windwalker vertical slice
Human regression
```

Any Core PR required by these gates is subordinate to the corresponding PlayerBot project Issue.

## 8. Rule for future contributors and assistants

**DO NOT switch the project hub to `MOP_V2_Repack` without an explicit new architecture/governance decision.**

Unless superseded by a documented ADR/governance change:

```text
Main project / docs / Issues / PlayerBot code:
  hilch1981-prog/mod-playerbots / mop-5.4.8-v2

Runtime Core integration / minimal Core PRs:
  hilch1981-prog/MOP_V2_Repack / playerbot-v2-poc
```

This rule is intentionally duplicated in README and source-baseline documentation so that repository ownership remains clear even when work resumes in a new session.
