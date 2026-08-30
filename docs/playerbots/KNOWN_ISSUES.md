# Chipa PlayerBot V2 Known Issues / Architecture Open Items

This document indexes unresolved architecture and implementation questions from Architecture v0.1. They are intentionally left open until evidence exists.

## AO-001 ~ AO-010 — Core / Adapter foundations

- **AO-001** Bot account concurrent-character login policy
- **AO-002** Need for a synchronous bot-login helper
- **AO-003** Packet interception requirements
- **AO-004** Scope of generic module framework
- **AO-005** Adapter granularity
- **AO-006** Command permission mapping
- **AO-007** Separate PlayerBot DB adoption phase
- **AO-008** Spell name resolver scope
- **AO-009** Group abstraction level
- **AO-010** LFG adapter timing

## AO-011 ~ AO-020 — AI / class architecture

- **AO-011** Exact official-engine donor scope vs minimal compatible reimplementation
- **AO-012** Digi `BotRotation.cpp` migration strategy
- **AO-013** SelfBot manual-cast suppression/defer window
- **AO-014** Pet AI phase/scope
- **AO-015** Exact specialization ID source/mapping
- **AO-016** Strategy command/name compatibility (`co`, `nc`, aliases)
- **AO-017** Action priority scale normalization
- **AO-018** Safe generic fallback AI scope
- **AO-019** MoP talent/glyph variation handling
- **AO-020** PvP rotation/strategy overlays

## AO-021 ~ AO-030 — DB / dataset architecture

- **AO-021** PlayerBot DB connection ownership: generic Core pool vs module-owned pool
- **AO-022** Phase when separate PlayerBot DB becomes required
- **AO-023** Development AutoMigrate support
- **AO-024** MoP 5.4.8 travel generator design
- **AO-025** File-by-file decision for legacy PR #389 datasets: reuse/adapt/regenerate/reject
- **AO-026** Bot-name dataset locale policy
- **AO-027** RandomBot generic event schema vs typed schema
- **AO-028** Cache persistence vs startup regeneration
- **AO-029** `utf8` vs `utf8mb4` compatibility validation
- **AO-030** Auto-created bot-account secret/password policy

## AO-031 ~ AO-040 — Scheduler / performance / fault handling

- **AO-031** AI scheduler implementation algorithm
- **AO-032** Exact per-tick PlayerBot work budget
- **AO-033** Core path/navmesh thread-safety boundaries
- **AO-034** Group-level spatial-cache value vs complexity
- **AO-035** Background RandomBot simulation fidelity
- **AO-036** Bot-only LFG/Dungeon policy
- **AO-037** Raid cooldown coordinator scope
- **AO-038** Automatic feature circuit-breaker policy
- **AO-039** Per-bot memory target
- **AO-040** L1000 acceptance: stress observation vs supported target

## AO-041 ~ AO-050 — Verification / release architecture

- **AO-041** How much verification to automate in CI
- **AO-042** Exact performance release thresholds
- **AO-043** Official supported/recommended bot count
- **AO-044** Automated in-game test harness
- **AO-045** Exact AI quality-grade criteria
- **AO-046** Final release-channel naming
- **AO-047** Upstream review cadence
- **AO-048** Compat API ABI/version strategy
- **AO-049** DB downgrade/rollback support level
- **AO-050** Conditions for Architecture v1.0 promotion

## Resolution rule

An AO is closed only by one or more of:

- source analysis
- POC evidence
- build/runtime/game test
- benchmark data
- operational decision
- a new/superseding ADR

Do not close an AO by guessing a production value before measurement.

## High-priority items for the first POC

The first SelfBot/Windwalker POC is expected to directly provide evidence for:

- AO-004 generic module framework scope
- AO-005 Adapter granularity
- AO-006 command permission mapping
- AO-011 AI-engine donor scope
- AO-012 Digi rotation migration strategy
- AO-013 SelfBot manual-input interaction
- AO-017 action priority behavior

ManagedBot POC later targets AO-001/002/003/009/010 and Core login/teleport questions.
