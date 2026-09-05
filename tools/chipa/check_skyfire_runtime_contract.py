#!/usr/bin/env python3
"""Verify the Chipa/SkyFire 5.4.8 API surface used by staged G2 code.

This is a cross-repository static compatibility contract. It intentionally
checks only the small Core-facing surface already consumed by the generic G2
bridge and the first donor source-closure adaptations. Passing this check is
not runtime evidence and must not promote G1 or G2.
"""

from pathlib import Path
import argparse
import sys


def read(root: Path, relative: str) -> str:
    path = root / relative
    if not path.is_file():
        raise AssertionError(f"missing runtime file: {relative}")
    return path.read_text(encoding="utf-8")


def require(text: str, token: str, where: str) -> None:
    if token not in text:
        raise AssertionError(f"{where}: missing required target API token: {token}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("runtime_root", type=Path)
    args = parser.parse_args()
    root = args.runtime_root.resolve()

    object_h = read(root, "src/server/game/Entities/Object/Object.h")
    log_h = read(root, "src/server/shared/Logging/Log.h")
    script_mgr_h = read(root, "src/server/game/Scripting/ScriptMgr.h")

    # Donor scheduler adaptation: SkyFire exposes packed/raw uint64 GUIDs and
    # a stable low-part accessor instead of AzerothCore ObjectGuid::GetCounter.
    require(object_h, "uint64 GetGUID() const", "Object.h GUID surface")
    require(object_h, "uint32 GetGUIDLow() const", "Object.h GUID surface")

    # PerfMonitor adaptation: SkyFire's logging surface is printf-varargs.
    require(log_h, "#define TC_LOG_INFO(filterType__, ...)", "Log.h logging surface")
    require(log_h, "void outInfo(std::string const& f, char const* str, ...) ATTR_PRINTF", "Log.h printf contract")

    # Generic PlayerScript seam used by the module-owned bridge.
    require(script_mgr_h, "virtual void OnLogin(Player* /*player*/) { }", "ScriptMgr.h PlayerScript login")
    require(script_mgr_h, "virtual void OnLogout(Player* /*player*/) { }", "ScriptMgr.h PlayerScript logout")
    require(script_mgr_h, "virtual void OnUpdate(Player* /*player*/, uint32 /*diff*/) { }", "ScriptMgr.h PlayerScript update")

    print("PASS: SkyFire runtime exposes uint64 GUID + GetGUIDLow scheduler surface")
    print("PASS: SkyFire runtime exposes TC_LOG_INFO printf-varargs logging surface")
    print("PASS: SkyFire PlayerScript exposes OnLogin/OnLogout/OnUpdate seams")
    print("NOTE: cross-repository static contract only; no runtime gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
