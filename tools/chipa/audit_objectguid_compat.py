#!/usr/bin/env python3
"""Inventory modern donor ObjectGuid API usage against Chipa/SkyFire 5.4.8.

This is an activation guard, not a compatibility PASS. SkyFire 5.4.8 has a
lightweight packet ObjectGuid wrapper in ByteBuffer.h plus uint64 object GUIDs;
it does not expose the modern AzerothCore ObjectGuid helper API used by parts
of the donor. While any known modern-only helper remains in the staged source
closure, the concrete donor backend must stay excluded from the module manifest
and bootstrap.
"""

from pathlib import Path
import argparse
import re
import sys

MODULE_ROOT = Path(__file__).resolve().parents[2]

DONOR_ROOTS = (
    "src/Script/Playerbots.cpp",
    "src/Bot/PlayerbotAI.cpp",
    "src/Bot/Engine/PlayerbotAIBase.cpp",
    "src/Bot/PlayerbotMgr.cpp",
)

MODERN_GUID_METHODS = (
    "ReadAsPacked",
    "IsPlayer",
    "GetCounter",
)


def read(root: Path, relative: str) -> str:
    path = root / relative
    if not path.is_file():
        raise AssertionError(f"missing required file: {relative}")
    return path.read_text(encoding="utf-8")


def require(text: str, token: str, where: str) -> None:
    if token not in text:
        raise AssertionError(f"{where}: missing required token: {token}")


def modern_guid_findings(relative: str, text: str) -> list[tuple[str, str, int]]:
    findings: list[tuple[str, str, int]] = []

    empty_count = text.count("ObjectGuid::Empty")
    if empty_count:
        findings.append((relative, "ObjectGuid::Empty", empty_count))

    # Audit modern helper calls only on variables declared as ObjectGuid. A
    # broad `.IsPlayer()` scan creates false positives for valid SkyFire
    # WorldObject/Unit calls such as `obj->IsPlayer()`.
    names = set(re.findall(r"\bObjectGuid\s+([A-Za-z_]\w*)\b", text))
    for name in sorted(names):
        for method in MODERN_GUID_METHODS:
            pattern = rf"\b{re.escape(name)}\s*\.\s*{method}\s*\("
            count = len(re.findall(pattern, text))
            if count:
                findings.append((relative, f"{name}.{method}()", count))

    return findings


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("runtime_root", type=Path)
    args = parser.parse_args()
    runtime_root = args.runtime_root.resolve()

    bytebuffer_h = read(runtime_root, "src/server/shared/Packets/ByteBuffer.h")
    object_h = read(runtime_root, "src/server/game/Entities/Object/Object.h")
    manifest = read(MODULE_ROOT, "chipa_module.cmake")
    bootstrap = read(MODULE_ROOT, "src/chipa/ModuleBootstrap.cpp")

    # Establish the actual target model: gameplay objects expose uint64 GUIDs,
    # while packet code has a small byte-addressable ObjectGuid wrapper and
    # target-native packed/masked GUID helpers.
    for token in (
        "struct ObjectGuid",
        "ObjectGuid(uint64 guid)",
        "operator uint64()",
        "void readPackGUID(uint64& guid)",
        "void appendPackGUID(uint64 guid)",
        "void ReadGuidMask",
        "void ReadGuidBytes",
    ):
        require(bytebuffer_h, token, "SkyFire ByteBuffer.h ObjectGuid surface")
    require(object_h, "uint64 GetGUID() const", "SkyFire Object.h GUID surface")
    require(object_h, "uint32 GetGUIDLow() const", "SkyFire Object.h GUID surface")

    findings: list[tuple[str, str, int]] = []
    for relative in DONOR_ROOTS:
        findings.extend(modern_guid_findings(relative, read(MODULE_ROOT, relative)))

    if findings:
        # Known incompatibilities still exist, so activation must remain staged.
        if "src/chipa/DonorPlayerbotBackend.cpp" in manifest:
            raise AssertionError("ObjectGuid blockers remain but donor backend is active in chipa_module.cmake")
        if "ConfigureDonorPlayerUpdateBackend();" in bootstrap:
            raise AssertionError("ObjectGuid blockers remain but donor backend is active in ModuleBootstrap.cpp")

    print("PASS: target ObjectGuid model verified from live SkyFire runtime sources")
    if findings:
        print("PENDING: modern donor ObjectGuid helpers still require target-native adaptation:")
        for relative, marker, count in findings:
            print(f"  {relative}: {marker} x{count}")
        print("PASS: donor backend remains inactive while ObjectGuid blockers exist")
    else:
        print("PASS: no audited modern-only ObjectGuid helper markers remain in root closure")
    print("NOTE: inventory/activation guard only; no runtime gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
