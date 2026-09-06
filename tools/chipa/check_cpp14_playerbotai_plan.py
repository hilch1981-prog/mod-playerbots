#!/usr/bin/env python3
"""Validate the reviewed C++14 adaptation slice for PlayerbotAI.

The checker builds an in-memory preview only. It pins exact donor fragments and
semantics-preserving C++14 replacements for every blocker currently inventoried
in PlayerbotAI.cpp, including the three std::size calls discovered after the
first root-closure audit. It does not activate the donor backend or promote a
runtime gate.
"""

from pathlib import Path
import sys

from audit_cpp14_root_closure import strip_cpp_non_code

MODULE_ROOT = Path(__file__).resolve().parents[2]
TARGET = MODULE_ROOT / "src/Bot/PlayerbotAI.cpp"

MAYBE_UNUSED_OLD = "void PlayerbotAI::UpdateAIInternal([[maybe_unused]] uint32 elapsed, bool minimal)"
MAYBE_UNUSED_NEW = "void PlayerbotAI::UpdateAIInternal(uint32 /*elapsed*/, bool minimal)"

STARTS_WITH_OLD = "message.starts_with(sPlayerbotAIConfig.toxicLinksPrefix)"
STARTS_WITH_NEW = (
    "message.compare(0, sPlayerbotAIConfig.toxicLinksPrefix.size(), "
    "sPlayerbotAIConfig.toxicLinksPrefix) == 0"
)

CHANNEL_LOOP_OLD = """    for (auto const& [key, channel] : cMgr->GetChannels())
    {
"""
CHANNEL_LOOP_NEW = """    for (auto const& channelEntry : cMgr->GetChannels())
    {
        Channel* channel = channelEntry.second;
"""

STRING_VIEW_OLD = "std::string_view const auraName = spellInfo->SpellName[0];"
STRING_VIEW_NEW = "std::string const& auraName = spellInfo->SpellName[0];"

SHARP_SIZE_OLD = "std::size(uPrioritizedSharpStoneIds)"
SHARP_SIZE_NEW = "uPrioritizedSharpStoneIds.size()"
WEIGHT_SIZE_OLD = "std::size(uPrioritizedWeightStoneIds)"
WEIGHT_SIZE_NEW = "uPrioritizedWeightStoneIds.size()"
QUEST_SIZE_OLD = "std::size(quest->RequiredItemId)"
QUEST_SIZE_NEW = "(sizeof(quest->RequiredItemId) / sizeof(quest->RequiredItemId[0]))"


def require_exactly_once(text: str, fragment: str, label: str) -> None:
    count = text.count(fragment)
    if count != 1:
        raise AssertionError(f"{label}: expected reviewed fragment exactly once, found {count}")


def count_code_marker(text: str, marker: str) -> int:
    return strip_cpp_non_code(text).count(marker)


def main() -> int:
    if not TARGET.is_file():
        raise AssertionError(f"missing donor source: {TARGET.relative_to(MODULE_ROOT)}")

    original = TARGET.read_text(encoding="utf-8")

    replacements = (
        (MAYBE_UNUSED_OLD, MAYBE_UNUSED_NEW, "maybe_unused parameter"),
        (STARTS_WITH_OLD, STARTS_WITH_NEW, "toxic-links starts_with"),
        (CHANNEL_LOOP_OLD, CHANNEL_LOOP_NEW, "channel structured binding"),
        (STRING_VIEW_OLD, STRING_VIEW_NEW, "string_view aura name"),
        (SHARP_SIZE_OLD, SHARP_SIZE_NEW, "sharp-stone std::size"),
        (WEIGHT_SIZE_OLD, WEIGHT_SIZE_NEW, "weight-stone std::size"),
        (QUEST_SIZE_OLD, QUEST_SIZE_NEW, "quest-array std::size"),
    )

    for old, _new, label in replacements:
        require_exactly_once(original, old, label)

    preview = original
    for old, new, _label in replacements:
        preview = preview.replace(old, new, 1)

    expected_deltas = {
        "[[maybe_unused]]": (1, 0),
        ".starts_with(": (1, 0),
        "for (auto const& [": (1, 0),
        "std::string_view": (1, 0),
        "std::size(": (3, 0),
    }
    for marker, expected in expected_deltas.items():
        actual = (count_code_marker(original, marker), count_code_marker(preview, marker))
        if actual != expected:
            raise AssertionError(
                f"unexpected PlayerbotAI blocker delta for {marker!r}: expected {expected}, got {actual}"
            )

    if MAYBE_UNUSED_NEW not in preview:
        raise AssertionError("unused elapsed parameter was not preserved as a named-out C++14 parameter")
    if STARTS_WITH_NEW not in preview:
        raise AssertionError("toxic-links prefix gate was not preserved")
    if "Channel* channel = channelEntry.second;" not in preview:
        raise AssertionError("channel mapped value was not preserved in the C++14 loop preview")
    if "if (channel->GetChannelId() == chanId)" not in preview:
        raise AssertionError("channel-id selection behavior was lost in the preview")
    if STRING_VIEW_NEW not in preview:
        raise AssertionError("aura-name comparison value was not preserved as std::string const&")
    if SHARP_SIZE_NEW not in preview or WEIGHT_SIZE_NEW not in preview or QUEST_SIZE_NEW not in preview:
        raise AssertionError("one or more std::size C++14 replacements were not applied")

    print("PASS: reviewed PlayerbotAI C++14 adaptation fragments still match exactly")
    print("PASS: preview removes starts_with/structured-binding/maybe_unused/string_view/std::size blockers")
    print("PASS: toxic-link prefix, channel selection, aura-name, and loop-bound intent remains represented")
    print("PENDING: preview only; donor source is not rewritten and no gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
