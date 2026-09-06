#!/usr/bin/env python3
"""Pin Chipa MoP 5.4.8 unit packet layouts used by staged PlayerBot code.

The donor's ObjectGuid::ReadAsPacked() assumption for SMSG_DISMOUNT is not
valid for the target runtime. This contract reads the authoritative
MOP_V2_Repack Unit.cpp producer and fails if its GUID mask/byte order drifts
from the staged compatibility reader.
"""

from pathlib import Path
import argparse
import re
import sys


def read(root: Path, relative: str) -> str:
    path = root / relative
    if not path.is_file():
        raise AssertionError(f"missing required runtime file: {relative}")
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


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("runtime_root", type=Path)
    args = parser.parse_args()
    runtime_root = args.runtime_root.resolve()

    unit_cpp = read(runtime_root, "src/server/game/Entities/Unit/Unit.cpp")
    bytebuffer_h = read(runtime_root, "src/server/shared/Packets/ByteBuffer.h")

    for token in ("void ReadGuidMask", "void ReadGuidBytes"):
        if token not in bytebuffer_h:
            raise AssertionError(f"ByteBuffer.h: missing target-native helper: {token}")

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

    print("PASS: live MoP 5.4.8 SMSG_DISMOUNT layout matches staged compatibility reader")
    print("NOTE: packet-layout contract only; donor backend remains inactive and no gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
