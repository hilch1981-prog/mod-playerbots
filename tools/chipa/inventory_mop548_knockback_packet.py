#!/usr/bin/env python3
"""Pin the live Chipa MoP 5.4.8 SMSG_MOVE_KNOCK_BACK producer and call site.

This contract reads Unit::SendMoveKnockBack from the authoritative
MOP_V2_Repack baseline, verifies the exact scalar/GUID wire order consumed by
the compatibility reader, and pins PlayerbotAI to the target-native reader.
Passing it is static packet-layout/call-site evidence only and must not promote
G1 or G2.
"""

from pathlib import Path
import argparse
import re
import sys

MODULE_ROOT = Path(__file__).resolve().parents[2]


def extract_function(text: str, signature: str) -> str:
    start = text.find(signature)
    if start < 0:
        raise AssertionError(f"missing runtime function: {signature}")

    brace = text.find("{", start)
    if brace < 0:
        raise AssertionError(f"missing opening brace for: {signature}")

    depth = 0
    for pos in range(brace, len(text)):
        char = text[pos]
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return text[start : pos + 1]

    raise AssertionError(f"unterminated runtime function: {signature}")


def compact(text: str) -> str:
    return re.sub(r"\s+", "", text)


def require_order(segment: str, tokens: list[str]) -> None:
    pos = 0
    for token in tokens:
        needle = compact(token)
        found = segment.find(needle, pos)
        if found < 0:
            raise AssertionError(f"SMSG_MOVE_KNOCK_BACK producer layout drift at token: {token}")
        pos = found + len(needle)


def require_once(text: str, token: str, where: str) -> None:
    count = text.count(token)
    if count != 1:
        raise AssertionError(f"{where}: expected exactly one `{token}`, found {count}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("runtime_root", type=Path)
    args = parser.parse_args()

    unit_cpp = args.runtime_root / "src/server/game/Entities/Unit/Unit.cpp"
    if not unit_cpp.is_file():
        raise AssertionError(f"missing runtime file: {unit_cpp}")

    text = unit_cpp.read_text(encoding="utf-8-sig")
    function = extract_function(
        text,
        "void Unit::SendMoveKnockBack(Player* player, float speedXY, float speedZ, float vcos, float vsin)",
    )
    segment = compact(function)

    if "SMSG_MOVE_KNOCK_BACK" not in function:
        raise AssertionError("SendMoveKnockBack no longer produces SMSG_MOVE_KNOCK_BACK")

    require_order(
        segment,
        [
            "data << float(speedXY);",
            "data << float(vsin);",
            "data << float(speedZ);",
            "data << uint32(0);",
            "data << float(vcos);",
            "data.WriteBit(guid[2]);",
            "data.WriteBit(guid[0]);",
            "data.WriteBit(guid[7]);",
            "data.WriteBit(guid[1]);",
            "data.WriteBit(guid[4]);",
            "data.WriteBit(guid[6]);",
            "data.WriteBit(guid[5]);",
            "data.WriteBit(guid[3]);",
            "data.WriteByteSeq(guid[6]);",
            "data.WriteByteSeq(guid[0]);",
            "data.WriteByteSeq(guid[7]);",
            "data.WriteByteSeq(guid[5]);",
            "data.WriteByteSeq(guid[4]);",
            "data.WriteByteSeq(guid[3]);",
            "data.WriteByteSeq(guid[1]);",
            "data.WriteByteSeq(guid[2]);",
        ],
    )

    playerbot_ai_path = MODULE_ROOT / "src/Bot/PlayerbotAI.cpp"
    if not playerbot_ai_path.is_file():
        raise AssertionError("missing module file: src/Bot/PlayerbotAI.cpp")
    playerbot_ai = playerbot_ai_path.read_text(encoding="utf-8-sig")

    require_once(
        playerbot_ai,
        "ReadMoveKnockBackForGuid(p, guid, bot->GetGUID(), counter, vcos, vsin,",
        "PlayerbotAI.cpp SMSG_MOVE_KNOCK_BACK",
    )
    if "guid.ReadAsPacked()" in playerbot_ai:
        raise AssertionError("PlayerbotAI.cpp: modern guid.ReadAsPacked() parser reintroduced")

    print("PASS: live MoP 5.4.8 SMSG_MOVE_KNOCK_BACK producer wire order is pinned")
    print("PASS: scalar order is speedXY -> vsin -> speedZ -> counter -> vcos before GUID mask/bytes")
    print("PASS: GUID mask order 2,0,7,1,4,6,5,3 and byte order 6,0,7,5,4,3,1,2")
    print("PASS: PlayerbotAI knockback call site uses the target-native identity-gated reader")
    print("PASS: PlayerbotAI no longer contains donor guid.ReadAsPacked() parsing")
    print("NOTE: static packet-layout/call-site evidence only; no runtime gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
