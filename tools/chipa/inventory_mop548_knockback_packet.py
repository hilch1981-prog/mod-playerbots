#!/usr/bin/env python3
"""Inventory the live Chipa MoP 5.4.8 SMSG_MOVE_KNOCK_BACK producer.

This intentionally does not encode an expected wire layout yet. It extracts
Unit::SendMoveKnockBack from the authoritative MOP_V2_Repack baseline and emits
the exact producer body as CI evidence so the remaining donor ReadAsPacked()
call site can be adapted without guessing. Once the layout is pinned, this
inventory is expected to become a strict compatibility contract.
"""

from pathlib import Path
import argparse
import sys


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

    if "SMSG_MOVE_KNOCK_BACK" not in function:
        raise AssertionError("SendMoveKnockBack no longer produces SMSG_MOVE_KNOCK_BACK")

    print("BEGIN_CHIPA_MOP548_KNOCKBACK_PRODUCER")
    print(function)
    print("END_CHIPA_MOP548_KNOCKBACK_PRODUCER")
    print("NOTE: inventory evidence only; no packet layout or gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
