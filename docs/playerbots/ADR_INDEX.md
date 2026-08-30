# Chipa PlayerBot V2 ADR Index

Architecture v0.1 produced **ADR-001 through ADR-180**.

This index groups the decisions by architecture part. The detailed rationale is consolidated in [`ARCHITECTURE.md`](ARCHITECTURE.md); future superseding decisions should be recorded without silently rewriting old decisions.

## ADR status model

- PROPOSED
- ACCEPTED
- SUPERSEDED
- DEPRECATED
- REJECTED

Current Architecture v0.1 decisions are treated as **ACCEPTED baseline decisions** unless a later ADR explicitly supersedes them.

## Part 1 — Architecture principles / boundaries

**ADR-001 ~ ADR-012**

Covers:

- Chipa runtime source of truth
- separate module architecture
- minimal Core bridge
- Compatibility Adapter
- official/Digi source roles
- class/spec separation
- SelfBot/ManagedBot AI reuse
- no direct WotLK spell/talent/opcode assumptions
- removable/optional module
- feature/PR isolation

## Part 2 — Runtime / lifecycle

**ADR-013 ~ ADR-024**

Covers:

- real Core `Player` for all bots
- no FakePlayer/NpcPlayer abstraction
- SelfBot/ManagedBot shared AI
- socketless ManagedBot session
- manager-owned bot sessions
- asynchronous bulk login
- server-side teleport completion
- normal Core spell/movement semantics
- throttled AI update
- destruction/shutdown order
- fail-soft lifecycle principles

## Part 3 — Core Bridge

**ADR-025 ~ ADR-038**

Covers:

- generic module framework
- compile-time vs runtime enable switches
- no Core→module reverse dependency
- generated module script registration
- reuse PlayerScript update
- minimal bot marker/session state
- Core-owned character-login internals
- async bulk login
- no fake teleport ACK packets
- generic Group/LFG read helpers only
- module-off regression
- Core modification budget/review

## Part 4 — Compatibility Adapter / API contracts

**ADR-039 ~ ADR-060**

Covers:

- selective Adapter, not whole-Core wrapper
- stable Core types allowed directly
- callback normalization
- session/login/movement/spell contracts
- no raw game-object pointer across unsafe async boundaries
- numeric MoP spell identity
- locale separation
- Chipa-native permissions/config
- DB repository/DTO boundary
- capability matrix and fail-soft behavior
- thread affinity
- provenance separation
- minimum SelfBot POC adapters

## Part 5 — AI Engine / 11 classes / 34 specs

**ADR-061 ~ ADR-090**

Covers:

- official PlayerBots AI model as long-term reference
- Digi thin MoP engine as initial execution donor
- separate class/spec rotation layer
- PlayerbotAI as orchestrator
- separate lifecycle/AI state
- Strategy/Trigger/Action/Value semantics
- relevance/multiplier/queue arbitration
- role derived from active specialization
- independent 34-spec registry
- generic combat services
- target/threat/healing separation
- SelfBot control policy
- no spell provisioning from combat rotation
- Monk as reference implementation
- context overlays for dungeon/raid/PvP
- no DB/string spell lookup in AI hot path

## Part 6 — Database / Config / Data

**ADR-091 ~ ADR-120**

Covers:

- Core DBs remain sources of truth
- separate logical PlayerBot DB target
- SelfBot POC without PlayerBot DB
- real AUTH accounts and CHARACTERS records
- explicit bot registry ownership
- PlayerBot data classification D0~D5
- config/DB/runtime separation
- legacy SQL not automatically MoP-valid
- MoP travel validation/regeneration
- dataset provenance/manifest
- schema/seed separation
- cache rebuildability
- no DB query in AI hot path
- repository boundary
- version/migration safety
- uninstall preservation
- utf8mb4/InnoDB preference for new PlayerBot tables
- fail-soft DB capability handling

## Part 7 — Advanced systems / performance / concurrency / fault isolation

**ADR-121 ~ ADR-150**

Covers:

- Human-first stability policy
- L100/L250/L500/L1000 as performance gates
- separate Quest/Loot/Vendor/Travel/LFG/PvP/Raid services
- conservative loot/vendor behavior
- hierarchical travel
- queued/budgeted path planning
- no unverified off-thread live Core pathfinding
- GUID/request/generation async identity
- group/raid shared contexts
- RandomBot population vs scheduler separation
- foreground/background scheduling
- budgeted scheduler and jitter
- event-driven updates
- HOT/WARM/COLD caching and reduced Grid scans
- shared immutable datasets/lazy initialization
- paced login/init
- load shedding
- game-thread mutation boundary
- quarantine/circuit-breaker/fault isolation
- sampled categorized logging
- multi-metric performance acceptance

## Part 8 — Verification / upstream / maintenance / release

**ADR-151 ~ ADR-180**

Covers:

- evidence-based completion
- V0~V9 verification lifecycle
- Architecture vs Feature gates
- Core-change justification gate
- minimal SelfBot/Windwalker first POC
- ManagedBot and RandomBot staged gates
- independent spec verification and quality grading
- dataset verification lifecycle
- module absent/disabled/enabled regression modes
- migration/failure injection tests
- measured performance thresholds
- L1000 stress semantics
- upstream selective porting
- mandatory code/dataset provenance
- commit-SHA baselines
- superseding ADR model
- AO evidence resolution
- separated Architecture/Module/Compat/DB/Dataset versions
- release maturity channels
- feature-based PR/test/release manifests
- Architecture v0.1 baseline freeze

## Superseding decisions

If a future POC or benchmark disproves one of these decisions:

1. keep the original ADR record
2. create a new ADR
3. mark the old ADR `SUPERSEDED`
4. link the POC/test evidence
5. update `ARCHITECTURE.md` and relevant implementation contract documents

Do not silently rewrite architecture history.
