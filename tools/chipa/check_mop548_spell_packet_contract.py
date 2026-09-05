#!/usr/bin/env python3
"""Pin Chipa MoP 5.4.8 spell packet layouts used by staged PlayerBot code.

The donor's modern ObjectGuid::ReadAsPacked() assumptions are not valid for
these 5.4.8 server->client packets. This contract reads the authoritative
MOP_V2_Repack Spell.cpp producer and fails if the bit/byte/scalar order used by
our staged compatibility reader no longer matches it.
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
        raise AssertionError(f"Spell.cpp: missing producer for {opcode}")
    end_marker = "m_caster->SendMessageToSet(&data, true);"
    end = text.find(end_marker, start)
    if end < 0:
        raise AssertionError(f"Spell.cpp: missing send boundary for {opcode}")
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

    spell_cpp = read(runtime_root, "src/server/game/Spells/Spell.cpp")
    bytebuffer_h = read(runtime_root, "src/server/shared/Packets/ByteBuffer.h")

    for token in ("void ReadGuidMask", "void ReadGuidBytes"):
        if token not in bytebuffer_h:
            raise AssertionError(f"ByteBuffer.h: missing target-native helper: {token}")

    failure = packet_slice(spell_cpp, "SMSG_SPELL_FAILURE")
    require_order(
        failure,
        "SMSG_SPELL_FAILURE",
        [
            "data.WriteBit(guid[7]);",
            "data.WriteBit(guid[3]);",
            "data.WriteBit(guid[6]);",
            "data.WriteBit(guid[2]);",
            "data.WriteBit(guid[1]);",
            "data.WriteBit(guid[5]);",
            "data.WriteBit(guid[0]);",
            "data.WriteBit(guid[4]);",
            "data.WriteByteSeq(guid[2]);",
            "data.WriteByteSeq(guid[6]);",
            "data.WriteByteSeq(guid[7]);",
            "data.WriteByteSeq(guid[0]);",
            "data.WriteByteSeq(guid[3]);",
            "data.WriteByteSeq(guid[1]);",
            "data << uint8(result);",
            "data << uint32(m_spellInfo->Id);",
            "data << uint8(m_cast_count);",
            "data.WriteByteSeq(guid[4]);",
            "data.WriteByteSeq(guid[5]);",
        ],
    )

    delayed = packet_slice(spell_cpp, "SMSG_SPELL_DELAYED")
    require_order(
        delayed,
        "SMSG_SPELL_DELAYED",
        [
            "data.WriteBit(guid[6]);",
            "data.WriteBit(guid[7]);",
            "data.WriteBit(guid[2]);",
            "data.WriteBit(guid[0]);",
            "data.WriteBit(guid[4]);",
            "data.WriteBit(guid[3]);",
            "data.WriteBit(guid[1]);",
            "data.WriteBit(guid[5]);",
            "data.WriteByteSeq(guid[2]);",
            "data.WriteByteSeq(guid[6]);",
            "data.WriteByteSeq(guid[1]);",
            "data.WriteByteSeq(guid[7]);",
            "data.WriteByteSeq(guid[0]);",
            "data.WriteByteSeq(guid[5]);",
            "data.WriteByteSeq(guid[3]);",
            "data << uint32(delaytime);",
            "data.WriteByteSeq(guid[4]);",
        ],
    )

    print("PASS: live MoP 5.4.8 spell packet producer layouts match staged compatibility readers")
    print("NOTE: packet-layout contract only; donor backend remains inactive and no gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
