#!/usr/bin/env python3
"""Pin the MoP 5.4.8 text-emote command layout consumed by Chipa runtime.

Playerbot donor PlayEmote() constructs a packet and calls
WorldSession::HandleTextEmoteOpcode directly. The modern donor writes a flat
ObjectGuid payload, while the target handler consumes a MoP-specific GUID
bit/byte sequence. This contract fails on runtime layout drift so the staged
writer cannot silently encode the wrong packet.
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


def function_slice(text: str, start_marker: str, end_marker: str) -> str:
    start = text.find(start_marker)
    if start < 0:
        raise AssertionError(f"missing target handler: {start_marker}")
    end = text.find(end_marker, start)
    if end < 0:
        raise AssertionError(f"missing handler boundary: {end_marker}")
    return compact(text[start:end])


def require_order(segment: str, tokens: list[str]) -> None:
    pos = 0
    for token in tokens:
        needle = compact(token)
        found = segment.find(needle, pos)
        if found < 0:
            raise AssertionError(f"HandleTextEmoteOpcode layout drift at token: {token}")
        pos = found + len(needle)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("runtime_root", type=Path)
    args = parser.parse_args()
    runtime_root = args.runtime_root.resolve()

    chat_cpp = read(runtime_root, "src/server/game/Handlers/ChatHandler.cpp")
    bytebuffer_h = read(runtime_root, "src/server/shared/Packets/ByteBuffer.h")

    for token in ("void WriteByteSeq", "void WriteBit"):
        if token not in bytebuffer_h:
            raise AssertionError(f"ByteBuffer.h: missing target packet writer surface: {token}")

    handler = function_slice(
        chat_cpp,
        "void WorldSession::HandleTextEmoteOpcode(WorldPacket& recvData)",
        "void WorldSession::HandleChatIgnoredOpcode",
    )

    require_order(
        handler,
        [
            "recvData >> text_emote;",
            "recvData >> emoteNum;",
            "guid[6] = recvData.ReadBit();",
            "guid[7] = recvData.ReadBit();",
            "guid[3] = recvData.ReadBit();",
            "guid[2] = recvData.ReadBit();",
            "guid[0] = recvData.ReadBit();",
            "guid[5] = recvData.ReadBit();",
            "guid[1] = recvData.ReadBit();",
            "guid[4] = recvData.ReadBit();",
            "recvData.ReadByteSeq(guid[0]);",
            "recvData.ReadByteSeq(guid[5]);",
            "recvData.ReadByteSeq(guid[1]);",
            "recvData.ReadByteSeq(guid[4]);",
            "recvData.ReadByteSeq(guid[2]);",
            "recvData.ReadByteSeq(guid[3]);",
            "recvData.ReadByteSeq(guid[7]);",
            "recvData.ReadByteSeq(guid[6]);",
            "sScriptMgr->OnPlayerTextEmote(GetPlayer(), text_emote, emoteNum, guid);",
        ],
    )

    print("PASS: live MoP 5.4.8 HandleTextEmoteOpcode input layout matches staged command writer")
    print("NOTE: packet-handler contract only; donor PlayEmote remains unactivated and no gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
