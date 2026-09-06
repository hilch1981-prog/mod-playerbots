#!/usr/bin/env python3
"""Pin Chipa MoP 5.4.8 spell packet layouts used by staged PlayerBot code.

The donor's modern ObjectGuid::ReadAsPacked() assumptions are not valid for
these 5.4.8 server->client packets. This contract reads the authoritative
MOP_V2_Repack Spell.cpp producer and fails if the bit/byte/scalar order used by
our staged compatibility reader no longer matches it. It also pins the current
PlayerbotAI call sites to those target-native readers so a later donor refresh
cannot silently restore the modern packed-GUID parser.
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


def require_once(text: str, token: str, where: str) -> None:
    count = text.count(token)
    if count != 1:
        raise AssertionError(f"{where}: expected exactly one `{token}`, found {count}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("runtime_root", type=Path)
    args = parser.parse_args()
    runtime_root = args.runtime_root.resolve()

    spell_cpp = read(runtime_root, "src/server/game/Spells/Spell.cpp")
    bytebuffer_h = read(runtime_root, "src/server/shared/Packets/ByteBuffer.h")
    playerbot_ai = read(MODULE_ROOT, "src/Bot/PlayerbotAI.cpp")

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

    require_once(playerbot_ai, '#include "Mop548SpellPacketCompat.h"', "PlayerbotAI.cpp spell compat include")
    require_once(
        playerbot_ai,
        "ReadSpellFailureForCaster(p, casterGuid, bot->GetGUID(), result, spellId, count)",
        "PlayerbotAI.cpp SMSG_SPELL_FAILURE",
    )
    require_once(
        playerbot_ai,
        "ReadSpellDelayedForCaster(p, casterGuid, bot->GetGUID(), delaytime)",
        "PlayerbotAI.cpp SMSG_SPELL_DELAYED",
    )
    if "p >> casterGuid.ReadAsPacked();" in playerbot_ai:
        raise AssertionError("PlayerbotAI.cpp: modern casterGuid.ReadAsPacked() parser reintroduced")

    print("PASS: live MoP 5.4.8 spell packet producer layouts match staged compatibility readers")
    print("PASS: PlayerbotAI spell call sites use target-native caster-gated readers")
    print("NOTE: packet-layout/call-site contract only; donor backend remains inactive and no gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
