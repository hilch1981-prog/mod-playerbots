# Chipa MoP 5.4.8 PlayerBot V2 — G2 donor source-closure roots

Status: **pre-activation evidence only**. This document does not promote G1 or G2.

## Why this inventory exists

`src/chipa/DonorPlayerbotBackend.cpp` now records the intended concrete G2 update wiring, but it is deliberately excluded from `chipa_module.cmake` and is not called by `ModuleBootstrap.cpp`. The donor backend cannot be activated safely by adding that one translation unit alone: the public symbols it calls are implemented inside the wider AzerothCore PlayerBots donor subsystem and pull a substantial dependency closure that has not yet been adapted to the Chipa/SkyFire 5.4.8 runtime.

The next development step is therefore to adapt and prove the minimum donor source closure incrementally instead of enabling the backend and treating linker/compiler failures as runtime evidence.

## Verified root symbols

| Required by staged backend | Donor declaration / implementation root | Current finding |
| --- | --- | --- |
| `PlayerbotsMgr::instance().GetPlayerbotAI(Player*)` | `src/Script/Playerbots.h` / `src/Script/Playerbots.cpp` | Direct map accessor exists. Use it instead of adding an AzerothCore `WorldSession::GetPlayerbotMgr()` Core patch. |
| `PlayerbotsMgr::instance().GetPlayerbotMgr(Player*)` | `src/Script/Playerbots.h` / `src/Script/Playerbots.cpp` | Direct map accessor exists and is the selected Chipa ownership lookup seam. |
| `PlayerbotAI::UpdateAI(uint32, bool)` | `src/Bot/PlayerbotAI.h` / `src/Bot/PlayerbotAI.cpp` | Public override exists. The staged backend invokes this public API; it must not call `UpdateAIInternal()` directly. |
| `PlayerbotMgr::UpdateAI(uint32, bool)` | inherited through `PlayerbotAIBase` | `PlayerbotMgr` derives through `PlayerbotHolder` from `PlayerbotAIBase`. Public `PlayerbotAIBase::UpdateAI()` is the scheduler entry point and dispatches to the virtual internal implementation. |
| `PlayerbotMgr::UpdateAIInternal(uint32, bool)` | `src/Bot/PlayerbotMgr.cpp` | Concrete manager work exists here, but is reached only through the inherited public `UpdateAI()` boundary. |

## Source-closure roots to adapt

These are **roots, not a claim that four files are the complete closure**:

1. `src/Script/Playerbots.cpp` — owns `PlayerbotsMgr` lookup/accessor implementation and donor script/world integration. This file also references PlayerBot configuration, random-bot management, world/session hooks, packet paths, and other donor services.
2. `src/Bot/PlayerbotAI.cpp` — owns concrete `PlayerbotAI::UpdateAI()`. Its include surface reaches AI factories/engines, movement, spell, guild/LFG, packet, configuration, performance, text and many other donor components.
3. `src/Bot/Engine/PlayerbotAIBase.cpp` — owns the inherited public `UpdateAI()` scheduler used by `PlayerbotMgr`; this scheduler performs delay/performance handling and then virtual-dispatches to `UpdateAIInternal()`.
4. `src/Bot/PlayerbotMgr.cpp` — owns manager/holder behavior and `PlayerbotMgr::UpdateAIInternal()`. It references session/login, character DB/cache, repository, random-bot, world-thread processor and related donor services.

Because these roots fan out into many donor symbols, blindly adding all upstream `src/**/*.cpp` to the Chipa manifest would destroy the explicit-adaptation boundary. The source closure must stay opt-in and reviewable.

## Canonical activation order

1. Keep `DonorPlayerbotBackend.cpp` staged and inactive while the root dependency closure is inventoried.
2. Adapt the smallest compile/link slice needed by the root symbols, one responsibility group at a time. Preserve provenance for every donor file changed for SkyFire/MoP compatibility.
3. Add static checks for Core API mismatches discovered during adaptation; do not patch Core merely to imitate AzerothCore convenience accessors when the module-owned/direct-map path is sufficient.
4. Build the adapted donor closure against `MOP_V2_Repack` with `MODULES=1`. Compiler and linker evidence is authoritative; static contract tests alone are not sufficient.
5. Only after the concrete donor backend itself compiles and links, add `src/chipa/DonorPlayerbotBackend.cpp` to `chipa_module.cmake` and call `ConfigureDonorPlayerUpdateBackend()` from the module bootstrap.
6. Re-run `MODULES=0` and `MODULES=1` whole-server builds and retain the exact runtime gitlink/module SHA evidence. Use Claude Code Review as a secondary review, not as a PASS source.
7. G2 still remains non-PASS until its runtime criteria are evidenced. G1-B3 remains independently pending until known-good Chipa DB/Data/config boot and human-client smoke are captured.

## Non-negotiable boundaries

- Generic Chipa bridge/adapter code stays free of `PlayerbotAI`, `PlayerbotMgr`, `WorldSession`, DB, random-bot and other concrete donor implementation headers.
- Concrete donor headers stay isolated to donor-specific compatibility translation units.
- Resolve AI and manager fresh for each dispatch. Do not cache their raw pointers across lifecycle/SelfBot transitions.
- Preserve public update order: **AI `UpdateAI(diff)` → manager `UpdateAI(diff)`**.
- Never call `UpdateAIInternal()` from the Chipa bridge/backend.
- Do not mark G1 or G2 PASS from this inventory, from static checks, or from AI review.

## Current activation blocker vs. physical-environment blocker

The donor source-closure adaptation is an engineering task that can continue autonomously and is **not** a user blocker. Separately, G1-B3 ultimately needs the user's known-good Chipa DB/Data/config and real 5.4.8 client environment for worldserver-ready plus login/movement/basic-combat evidence. That physical test does not block source-closure adaptation, CI preparation, or review work.