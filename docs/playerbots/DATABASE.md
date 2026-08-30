# Chipa PlayerBot V2 Database / Config / Dataset Architecture

Target: WoW Mists of Pandaria 5.4.8 Build 18414

## Logical ownership

```text
AUTH       → real accounts/security
CHARACTERS → real characters and character state
WORLD      → real world/game data
PLAYERBOTS → PlayerBot-specific persistent/reference/cache/generated data
```

The PlayerBot project does not duplicate Core account/character/world state into private tables.

## Data classes

- **D0 Core-owned** — account, character, spell, quest, inventory, map, guild/group state
- **D1 PlayerBot persistent** — preferences, custom strategy/profile, RandomBot ownership/state
- **D2 PlayerBot reference** — speech, weights, dungeon/travel metadata
- **D3 rebuildable cache** — item/equipment/rarity/tele caches
- **D4 generated dataset** — generated MoP travel graph, generated weights, generated names
- **D5 runtime-only** — target, action queue, trigger/value cache, movement intent

Runtime AI state must not be written to DB each tick.

## PlayerBot DB

A separate logical `playerbots` database is the long-term target for:

- bot account registry
- bot character registry
- preferred mounts / bot preferences
- custom strategies/profiles
- RandomBot state/scheduling
- speech data
- travel nodes/links/paths
- item/equipment/rarity/tele caches
- weight definitions/data
- dataset manifest
- schema version

The first SelfBot POC must not depend on this database.

## Account / character rules

- Real accounts live in AUTH.
- Real characters live in CHARACTERS.
- Prefixes such as `RNDBOT` are naming/discovery hints only, not ownership authority.
- PlayerBot-created accounts/characters should be tracked by registry metadata.
- Wipe/delete operations must act on explicit registry membership and require confirmation/dry-run where possible.
- Removing the module must not automatically delete accounts or characters.

## Config vs DB vs runtime

- **Config** = server-wide operator policy (`Playerbots.Enable`, scheduler limits, feature switches)
- **DB** = durable per-bot state or structured datasets
- **Runtime memory** = what the bot is doing right now

Do not turn config files into a per-bot database, and do not store every server-wide option in DB.

## Config namespace

Recommended structure:

```text
Playerbots.Enable
Playerbots.Database.*
Playerbots.Session.*
Playerbots.SelfBot.*
Playerbots.ManagedBots.*
Playerbots.RandomBots.*
Playerbots.AutoCreate.*
Playerbots.Init.*
Playerbots.Combat.*
Playerbots.Healing.*
Playerbots.Movement.*
Playerbots.Quest.*
Playerbots.Loot.*
Playerbots.Travel.*
Playerbots.LFG.*
Playerbots.PvP.*
Playerbots.Gear.*
Playerbots.Locale.*
Playerbots.Logging.*
Playerbots.Debug.*
Playerbots.Safety.*
Playerbots.Scheduler.*
Playerbots.Performance.*
Playerbots.LoadShedding.*
```

Configuration is loaded/validated into a runtime snapshot. AI hot paths do not repeatedly read config files/managers.

## Schema / migration layout

Recommended:

```text
sql/playerbots/
├─ base/
├─ updates/
├─ archive/
├─ seed/
└─ tools/
```

Schema and large seed datasets are separate.

Production runtime should verify schema rather than silently performing arbitrary DDL by default.

Schema states:

- expected == current → PASS
- DB older → MIGRATION_REQUIRED
- DB newer than module → MODULE_TOO_OLD / DB-dependent feature disabled
- DB unavailable → DB-dependent features disabled where possible

Persistent-data migrations and rebuildable-cache migrations have different recovery policies.

## Dataset manifest

Every large/imported/generated dataset should record:

- dataset name/version
- expansion/client build
- source repository/file or generator
- source commit/hash
- generator version
- content hash
- generated/imported date
- verification state

## Dataset verification

Suggested lifecycle:

```text
DVS0 Missing
DVS1 Imported
DVS2 Source Identified
DVS3 Static Validated
DVS4 MoP Adapted
DVS5 Runtime Validated
DVS6 Game Verified
DVS7 Release Verified
```

## Travel data policy

Travel-engine schema/algorithms from historical projects are useful references, but old coordinates/topology are not assumed MoP-compatible.

MoP Travel data must be validated or regenerated from Chipa MoP world/map/nav/transport information.

## Cache policy

Caches are not sources of truth. They can be invalidated/rebuilt when:

- World DB revision changes
- PlayerBot algorithm/version changes
- expansion/build changes
- weight/item evaluation changes

## DB access boundary

```text
AI / Service
   ↓
Repository
   ↓
BotDatabaseCompat
   ↓
DB
```

Repository returns PlayerBot DTO/domain data, not raw QueryResult/prepared statement objects.

No DB query is permitted in the combat AI hot path.

## Charset / storage

For new PlayerBot-only text tables, `utf8mb4` is preferred where supported. Do not mass-convert existing Core DBs for PlayerBot.

New PlayerBot tables default to InnoDB unless a measured reason justifies otherwise.

## Backup / uninstall

Before destructive schema changes, backup guidance is required.

Default module removal:

- preserve PlayerBot DB
- preserve actual bot accounts
- preserve actual bot characters

Full uninstall/data destruction is a separate explicit administrative operation.
