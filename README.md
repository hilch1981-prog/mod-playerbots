# Chipa PlayerBot V2 — MoP 5.4.8 Build 18414

This branch is the **main development hub** for the Chipa PlayerBot V2 project targeting **World of Warcraft: Mists of Pandaria 5.4.8 (Build 18414)**.

> **CANONICAL PROJECT HUB:** `hilch1981-prog/mod-playerbots`  
> **Primary development branch:** `mop-5.4.8-v2`  
> **All PlayerBot V2 project Issues / feature tracking / design / module code live here by default.**  
> **Runtime / integration core only:** [`hilch1981-prog/MOP_V2_Repack`](https://github.com/hilch1981-prog/MOP_V2_Repack)  
> **Core integration branch:** [`playerbot-v2-poc`](https://github.com/hilch1981-prog/MOP_V2_Repack/tree/playerbot-v2-poc)

## Project status

- Architecture v0.1: **complete — 8/8 parts**
- Architecture decisions: **ADR-001 ~ ADR-180**
- Open architecture items: **AO-001 ~ AO-050**
- POC specification: **complete**
- Current phase: **POC-G1 implementation / build validation**
- First POC target: **SelfBot + Monk Windwalker**

## Repository roles

| Repository | Role |
|---|---|
| **`hilch1981-prog/mod-playerbots` / `mop-5.4.8-v2`** | **Canonical PlayerBot V2 project hub: module source, docs, Issues, POC/feature tracking, validation tracking, release notes** |
| `hilch1981-prog/MOP_V2_Repack` / `playerbot-v2-poc` | Runtime Core integration, compile/boot/game/regression validation, and minimal generic Core bridge PRs only |
| `DigiD702/mod-playerbots` / `main` | Primary MoP 5.4.8 implementation donor: SelfBot, Monk, rotations, MoP AI behavior |
| `mod-playerbots/mod-playerbots` / `master` | Generic PlayerBot AI/features upstream reference |
| `DigiD702/skyfire_548_playerbots` | MoP core-hook reference for socketless sessions, bot login and teleport completion |
| Legends of Azeroth PR #389 | Historical MoP port/reference only |

See [`docs/playerbots/PROJECT_GOVERNANCE.md`](docs/playerbots/PROJECT_GOVERNANCE.md) for the mandatory repository/Issue/PR ownership rules.

## Development rules

1. `hilch1981-prog/mod-playerbots / mop-5.4.8-v2` is the **PlayerBot V2 project Source of Truth**.
2. Project Issues, feature tracking, architecture, POC tracking and PlayerBot module code belong here by default.
3. `MOP_V2_Repack` remains the **runtime Core Source of Truth**, not the PlayerBot project-management hub.
4. PlayerBot AI, rotations, lifecycle policy and feature logic belong in this module project.
5. Core changes must be minimal infrastructure bridges only and are tracked from a PlayerBot Issue.
6. External source is ported **feature-by-feature**, never by wholesale merge.
7. WotLK spell IDs, talents, opcodes and datasets are not accepted without MoP 5.4.8 validation.
8. SelfBot and ManagedBot share AI/rotation; lifecycle and control ownership differ.
9. MoP class AI is tracked by **11 classes / 34 specializations**.
10. Source presence is not completion: build, runtime, game and regression evidence are required.

## Initial POC gates

1. Generic module registration
2. PlayerScript `OnUpdate` → PlayerBot manager bridge
3. SelfBot attach / detach
4. Monk Windwalker minimal combat rotation
5. Human-player regression verification

The first POC intentionally excludes RandomBot, Travel, full Quest AI, LFG, Raid, Battleground and the full 34-spec rollout.

## Source baseline

The first frozen MoP donor baseline is:

- DigiD702/mod-playerbots
- branch: `main`
- commit: `13bc0ffa93c6b6625ed28fe2a03e0c071215ff48`

The Chipa runtime baseline used when the POC integration branch was created is:

- hilch1981-prog/MOP_V2_Repack
- branch: `repack-main`
- commit: `0739d072f8f1f42523f04cca4b2607d88a01def4`

See [`docs/playerbots/SOURCE_BASELINE.md`](docs/playerbots/SOURCE_BASELINE.md) for the full source policy.

## Core integration tracking

Core integration changes are subordinate to PlayerBot project tracking. The current runtime-core bootstrap is:

- [`MOP_V2_Repack PR #1 — POC-G1 Generic Module Loader`](https://github.com/hilch1981-prog/MOP_V2_Repack/pull/1)

This Core PR is an integration artifact. It does not replace this repository as the main PlayerBot V2 project location.

## 한국어 안내

**앞으로 치파 PlayerBot V2 작업의 메인은 이 저장소입니다.**

- 설계 / 문서 / Issue / 기능 추적 / PlayerBot 소스: **`hilch1981-prog/mod-playerbots / mop-5.4.8-v2`**
- 치파팩 Core 적용 / 컴파일 / 부팅 / 게임 회귀 검증: **`MOP_V2_Repack / playerbot-v2-poc`**
- Core 수정이 필요하더라도 해당 작업의 소유/추적 Issue는 PlayerBot 저장소에 둡니다.
- `MOP_V2_Repack`은 PlayerBot 프로젝트 허브로 바꾸지 않습니다.

이 규칙은 [`PROJECT_GOVERNANCE.md`](docs/playerbots/PROJECT_GOVERNANCE.md)에 고정되어 있으며, 명시적인 새 결정 없이는 변경하지 않습니다.

---

## Upstream acknowledgement

This repository originated as a fork of the official [`mod-playerbots/mod-playerbots`](https://github.com/mod-playerbots/mod-playerbots) project. The original `master` branch is retained as an upstream/reference line. The `mop-5.4.8-v2` branch is the Chipa MoP 5.4.8 adaptation line.
