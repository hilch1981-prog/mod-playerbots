#!/usr/bin/env python3
"""Pin Chipa MoP 5.4.8 unit packet layouts used by staged PlayerBot code.

The donor's modern ObjectGuid assumptions are not valid for the target runtime.
This contract reads authoritative MOP_V2_Repack producers and fails if their
wire layout or target-native GUID classification surface drifts from the staged
compatibility readers. It also pins the current PlayerbotAI dismount call site
to the target-native reader so donor packed-GUID parsing cannot regress silently.
"""

from pathlib import Path
import argparse
import re
import sys

MODULE_ROOT = Path(__file__).resolve().parents[2]


def read(root: Path, relative: str) -> str:
    path = root / relative
    if not path.is_file():
        raise AssertionError(f"missing required file: {relative}")
    return path.read_text(encoding="utf-8-sig")


def compact(text: str) -> str:
    return re.sub(r"\s+", "", text)


def packet_slice(text: str, opcode: str) -> str:
    start_marker = f"WorldPacket data({opcode}"
    start = text.find(start_marker)
    if start < 0:
        raise AssertionError(f"Unit.cpp: missing producer for {opcode}")
    end_marker = "SendMessageToSet(&data, true);"
    end = text.find(end_marker, start)
    if end < 0:
        raise AssertionError(f"Unit.cpp: missing send boundary for {opcode}")
    return compact(text[start : end + len(end_marker)])


def require_order(segment: str, opcode: str, tokens: list[str]) -> None:
    pos = 0
    for token in tokens:
        needle = compact(token)
        found = segment.find(needle, pos)
        if found < 0:
            raise AssertionError(f"{opcode}: producer layout drift at token: {token}")
        pos = found + len(needle)


def require_once(text: str, token: str, where: str) -> None:
    count = text.count(token)
    if count != 1:
        raise AssertionError(f"{where}: expected exactly one `{token}`, found {count}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("runtime_root", type=Path)
    args = parser.parse_args()
    runtime_root = args.runtime_root.resolve()

    unit_cpp = read(runtime_root, "src/server/game/Entities/Unit/Unit.cpp")
    bytebuffer_h = read(runtime_root, "src/server/shared/Packets/ByteBuffer.h")
    object_defines_h = read(runtime_root, "src/server/game/Entities/Object/ObjectDefines.h")
    playerbot_ai = read(MODULE_ROOT, "src/Bot/PlayerbotAI.cpp")

    for token in ("void ReadGuidMask", "void ReadGuidBytes"):
        if token not in bytebuffer_h:
            raise AssertionError(f"ByteBuffer.h: missing target-native helper: {token}")

    if "bool IS_PLAYER_GUID(uint64 guid)" not in object_defines_h:
        raise AssertionError("ObjectDefines.h: missing target-native IS_PLAYER_GUID(uint64) predicate")

    dismount = packet_slice(unit_cpp, "SMSG_DISMOUNT")
    require_order(
        dismount,
        "SMSG_DISMOUNT",
        [
            "data.WriteBit(guid[6]);",
            "data.WriteBit(guid[3]);",
            "data.WriteBit(guid[0]);",
            "data.WriteBit(guid[7]);",
            "data.WriteBit(guid[1]);",
            "data.WriteBit(guid[2]);",
            "data.WriteBit(guid[5]);",
            "data.WriteBit(guid[4]);",
            "data.WriteByteSeq(guid[3]);",
            "data.WriteByteSeq(guid[6]);",
            "data.WriteByteSeq(guid[7]);",
            "data.WriteByteSeq(guid[5]);",
            "data.WriteByteSeq(guid[1]);",
            "data.WriteByteSeq(guid[4]);",
            "data.WriteByteSeq(guid[2]);",
            "data.WriteByteSeq(guid[0]);",
        ],
    )

    emote = packet_slice(unit_cpp, "SMSG_EMOTE")
    require_order(
        emote,
        "SMSG_EMOTE",
        [
            "data << uint32(emote->Id);",
            "data << uint64(GetGUID());",
        ],
    )

    require_once(playerbot_ai, '#include "Mop548UnitPacketCompat.h"', "PlayerbotAI.cpp unit compat include")
    require_once(
        playerbot_ai,
        "ReadDismountForGuid(p, guid, bot->GetGUID())",
        "PlayerbotAI.cpp SMSG_DISMOUNT",
    )

    print("PASS: live MoP 5.4.8 SMSG_DISMOUNT layout matches staged compatibility reader")
    print("PASS: PlayerbotAI dismount call site uses target-native identity-gated reader")
    print("PASS: live MoP 5.4.8 SMSG_EMOTE layout is uint32 emote id -> raw uint64 GUID")
    print("PASS: target-native IS_PLAYER_GUID(uint64) predicate is available for donor adaptation")
    print("NOTE: packet-layout/call-site contract only; donor backend remains inactive and no gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
