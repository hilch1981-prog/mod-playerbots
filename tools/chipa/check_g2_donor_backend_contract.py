#!/usr/bin/env python3
"""Static contract for the staged Chipa MoP 5.4.8 donor G2 backend.

This checker proves concrete donor wiring and activation safety only. The donor
backend is intentionally excluded from the runtime manifest until its source
closure is adapted and whole-server build/link evidence exists. It must not be
used to promote G1 or G2.
"""

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]


def read(relative: str) -> str:
    path = ROOT / relative
    if not path.is_file():
        raise AssertionError(f"missing required file: {relative}")
    return path.read_text(encoding="utf-8")


def require(text: str, token: str, where: str) -> None:
    if token not in text:
        raise AssertionError(f"{where}: missing required token: {token}")


def forbid(text: str, token: str, where: str) -> None:
    if token in text:
        raise AssertionError(f"{where}: forbidden token present: {token}")


def require_order(text: str, tokens: tuple[str, ...], where: str) -> None:
    positions = []
    for token in tokens:
        require(text, token, where)
        positions.append(text.index(token))
    if positions != sorted(positions):
        raise AssertionError(f"{where}: required ordering violated: {' -> '.join(tokens)}")


def main() -> int:
    backend = read("src/chipa/DonorPlayerbotBackend.cpp")
    manifest = read("chipa_module.cmake")
    bootstrap = read("src/chipa/ModuleBootstrap.cpp")
    donor_ai_h = read("src/Bot/PlayerbotAI.h")
    donor_mgr_h = read("src/Bot/PlayerbotMgr.h")
    donor_base_h = read("src/Bot/Engine/PlayerbotAIBase.h")
    donor_base_cpp = read("src/Bot/Engine/PlayerbotAIBase.cpp")
    donor_perf_cpp = read("src/Bot/Debug/PerfMonitor.cpp")

    for token in (
        '#include "FreshResolvedBackend.h"',
        '#include "../Bot/PlayerbotAI.h"',
        '#include "../Bot/PlayerbotMgr.h"',
        "PlayerbotAI* ResolvePlayerbotAI(Player* player)",
        "PlayerbotsMgr::instance().GetPlayerbotAI(player)",
        "PlayerbotMgr* ResolvePlayerbotManager(Player* player)",
        "PlayerbotsMgr::instance().GetPlayerbotMgr(player)",
        "typedef FreshResolvedBackend<",
        "void ConfigureDonorPlayerUpdateBackend()",
        "ConfigurePlayerUpdateBackend(DonorPlayerUpdateBackend::MakeBackend());",
    ):
        require(backend, token, "DonorPlayerbotBackend.cpp")

    require_order(
        backend,
        (
            "PlayerbotAI* ResolvePlayerbotAI(Player* player)",
            "void ApplyPlayerbotAIUpdate(PlayerbotAI* ai, std::uint32_t diff)",
            "PlayerbotMgr* ResolvePlayerbotManager(Player* player)",
            "void ApplyPlayerbotManagerUpdate(PlayerbotMgr* manager, std::uint32_t diff)",
        ),
        "DonorPlayerbotBackend.cpp",
    )
    require(backend, "ai->UpdateAI(diff);", "DonorPlayerbotBackend.cpp")
    require(backend, "manager->UpdateAI(diff);", "DonorPlayerbotBackend.cpp")
    forbid(backend, "UpdateAIInternal", "DonorPlayerbotBackend.cpp")
    forbid(backend, "WorldSession", "DonorPlayerbotBackend.cpp")
    forbid(backend, "static PlayerbotAI*", "DonorPlayerbotBackend.cpp")
    forbid(backend, "static PlayerbotMgr*", "DonorPlayerbotBackend.cpp")

    # Verify the concrete calls still target public donor APIs rather than an
    # invented shim. PlayerbotMgr inherits the public UpdateAI entry point from
    # PlayerbotAIBase; its override remains UpdateAIInternal only.
    require(donor_ai_h, "void UpdateAI(uint32 elapsed, bool minimal = false) override;", "PlayerbotAI.h")
    require(donor_base_h, "virtual void UpdateAI(uint32 elapsed, bool minimal = false);", "PlayerbotAIBase.h")
    require(donor_mgr_h, "class PlayerbotMgr : public PlayerbotHolder", "PlayerbotMgr.h")
    require(donor_mgr_h, "PlayerbotAI* GetPlayerbotAI(Player* player);", "PlayerbotMgr.h")
    require(donor_mgr_h, "PlayerbotMgr* GetPlayerbotMgr(Player* player);", "PlayerbotMgr.h")

    # First concrete source-closure adaptation: the donor's modern AzerothCore
    # ObjectGuid::GetCounter() API does not exist in the Chipa/SkyFire 5.4.8
    # runtime, where Object::GetGUID() is uint64 and GetGUIDLow() is the stable
    # low-part accessor. Keep the deterministic scheduler staggering while
    # preventing this known Core-API mismatch from regressing.
    require(donor_base_cpp, "bot->GetGUIDLow() % 201", "PlayerbotAIBase.cpp SkyFire GUID adaptation")
    forbid(donor_base_cpp, "GetGUID().GetCounter()", "PlayerbotAIBase.cpp SkyFire GUID adaptation")

    # Keep the scheduler closure narrow. It needs only the performance monitor
    # API in addition to PlayerbotAIBase.h; pulling Script/Playerbots.h here
    # drags AI, manager, random-bot, spell and travel headers into this unit and
    # makes the MoP compatibility closure much harder to reason about.
    require(donor_base_cpp, '#include "PerfMonitor.h"', "PlayerbotAIBase.cpp narrow perf dependency")
    forbid(donor_base_cpp, '#include "Playerbots.h"', "PlayerbotAIBase.cpp narrow perf dependency")

    # Chipa/SkyFire 5.4.8 uses TC_LOG_* with printf-style varargs, while the
    # modern donor PerfMonitor used AzerothCore LOG_INFO with fmt placeholders.
    # Keep this closure on target-native logging and narrow direct dependencies.
    require(donor_perf_cpp, '#include "Log.h"', "PerfMonitor.cpp SkyFire logging")
    require(donor_perf_cpp, '#include "PlayerbotAIConfig.h"', "PerfMonitor.cpp config dependency")
    require(donor_perf_cpp, "TC_LOG_INFO(", "PerfMonitor.cpp SkyFire logging")
    require(donor_perf_cpp, "%7.3f%%", "PerfMonitor.cpp printf formatting")
    forbid(donor_perf_cpp, '#include "Playerbots.h"', "PerfMonitor.cpp narrow dependencies")
    if re.search(r"(?<!TC_)LOG_INFO\(", donor_perf_cpp):
        raise AssertionError("PerfMonitor.cpp SkyFire logging: legacy donor LOG_INFO call remains")
    if re.search(r'"[^"\n]*\{:[^"\n]*"', donor_perf_cpp):
        raise AssertionError("PerfMonitor.cpp SkyFire logging: fmt-style placeholder remains")

    # Activation is a separate gate. Until the donor source closure is adapted,
    # compiling this TU would introduce unresolved/incompatible donor symbols.
    # Keep both the manifest and bootstrap inactive so current G1 evidence is
    # not contaminated by an unproven backend.
    forbid(manifest, "src/chipa/DonorPlayerbotBackend.cpp", "chipa_module.cmake staged activation")
    forbid(bootstrap, "ConfigureDonorPlayerUpdateBackend();", "ModuleBootstrap.cpp staged activation")

    print("PASS: staged donor backend uses fresh PlayerbotsMgr accessors")
    print("PASS: staged donor backend preserves public AI -> manager update contract")
    print("PASS: no UpdateAIInternal/session dependency/raw-pointer cache introduced")
    print("PASS: PlayerbotAIBase scheduler uses SkyFire-compatible GetGUIDLow() staggering")
    print("PASS: PlayerbotAIBase scheduler keeps a narrow PerfMonitor-only dependency")
    print("PASS: PerfMonitor uses SkyFire TC_LOG_INFO printf logging with narrow dependencies")
    print("PASS: donor backend remains inactive pending source-closure build evidence")
    print("NOTE: static staging evidence only; no runtime gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
