# Chipa PlayerBot V2 — MoP 5.4.8 Build 18414

This branch is the **main development hub** for the Chipa PlayerBot V2 project targeting **World of Warcraft: Mists of Pandaria 5.4.8 (Build 18414)**.

> **Primary development repository:** `hilch1981-prog/mod-playerbots`  
> **Primary development branch:** `mop-5.4.8-v2`  
> **Runtime / integration core:** [`hilch1981-prog/MOP_V2_Repack`](https://github.com/hilch1981-prog/MOP_V2_Repack)  
> **Core integration branch:** [`playerbot-v2-poc`](https://github.com/hilch1981-prog/MOP_V2_Repack/tree/playerbot-v2-poc)

## Project status

- Architecture v0.1: **complete — 8/8 parts**
- Architecture decisions: **ADR-001 ~ ADR-180**
- Open architecture items: **AO-001 ~ AO-050**
- Current phase: **POC preparation**
- First POC target: **SelfBot + Monk Windwalker**

## Repository roles

| Repository | Role |
|---|---|
| **`hilch1981-prog/mod-playerbots` / `mop-5.4.8-v2`** | **Main PlayerBot V2 project hub and module development branch** |
| `hilch1981-prog/MOP_V2_Repack` / `playerbot-v2-poc` | Chipa MoP 5.4.8 runtime core integration, build and regression validation |
| `DigiD702/mod-playerbots` / `main` | Primary MoP 5.4.8 implementation donor: SelfBot, Monk, rotations, MoP AI behavior |
| `mod-playerbots/mod-playerbots` / `master` | Generic PlayerBot AI/features upstream reference |
| `DigiD702/skyfire_548_playerbots` | MoP core-hook reference for socketless sessions, bot login and teleport completion |
| Legends of Azeroth PR #389 | Historical MoP port/reference only |

## Development rules

1. `MOP_V2_Repack` remains the **runtime Core source of truth**.
2. PlayerBot AI, rotations, lifecycle policy and feature logic belong in this module project.
3. Core changes must be minimal infrastructure bridges only.
4. External source is ported **feature-by-feature**, never by wholesale merge.
5. WotLK spell IDs, talents, opcodes and datasets are not accepted without MoP 5.4.8 validation.
6. SelfBot and ManagedBot share AI/rotation; lifecycle and control ownership differ.
7. MoP class AI is tracked by **11 classes / 34 specializations**.
8. Source presence is not completion: build, runtime, game and regression evidence are required.

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

The runtime-core bootstrap is tracked in:

- [`MOP_V2_Repack PR #1 — Bootstrap PlayerBot V2 POC source baseline`](https://github.com/hilch1981-prog/MOP_V2_Repack/pull/1)

That PR is intentionally separated from feature implementation.

## 한국어 안내

이 브랜치가 **치파 PlayerBot V2의 메인 개발 허브**입니다.

- 실제 PlayerBot 기능 개발·설계·포팅 추적: **이 저장소 `mop-5.4.8-v2`**
- 치파팩 Core 적용·컴파일·실행·회귀 테스트: **`MOP_V2_Repack / playerbot-v2-poc`**
- MoP 구현 1차 참고: **DigiD702/mod-playerbots**
- 범용 PlayerBot 기능 참고: **공식 mod-playerbots**
- PR #389: 과거 MoP 포팅 비교 자료

즉 다른 개발자는 앞으로 이 저장소를 PlayerBot V2 프로젝트의 시작점으로 보면 됩니다.

---

## Upstream acknowledgement

This repository originated as a fork of the official [`mod-playerbots/mod-playerbots`](https://github.com/mod-playerbots/mod-playerbots) project. The original `master` branch is retained as an upstream/reference line. The `mop-5.4.8-v2` branch is the Chipa MoP 5.4.8 adaptation line.
