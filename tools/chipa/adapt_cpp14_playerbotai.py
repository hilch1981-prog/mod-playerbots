#!/usr/bin/env python3
"""Apply or verify the reviewed C++14 adaptation slice for PlayerbotAI.cpp.

This all-or-nothing transform covers only the exact donor fragments pinned by
the PlayerbotAI C++14 plan. Partial or drifted source fails rather than being
rewritten heuristically. Passing it is source compatibility evidence only and
must not promote G1 or G2.
"""

from pathlib import Path
import argparse
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

PAIRS = (
    ("maybe_unused parameter", MAYBE_UNUSED_OLD, MAYBE_UNUSED_NEW),
    ("toxic-links starts_with", STARTS_WITH_OLD, STARTS_WITH_NEW),
    ("channel structured binding", CHANNEL_LOOP_OLD, CHANNEL_LOOP_NEW),
    ("string_view aura name", STRING_VIEW_OLD, STRING_VIEW_NEW),
    ("sharp-stone std::size", SHARP_SIZE_OLD, SHARP_SIZE_NEW),
    ("weight-stone std::size", WEIGHT_SIZE_OLD, WEIGHT_SIZE_NEW),
    ("quest-array std::size", QUEST_SIZE_OLD, QUEST_SIZE_NEW),
)

TARGETED_MARKERS = (
    "[[maybe_unused]]",
    ".starts_with(",
    "for (auto const& [",
    "std::string_view",
    "std::size(",
)


def classify_state(text: str) -> str:
    old_counts = [text.count(old) for _, old, _ in PAIRS]
    new_counts = [text.count(new) for _, _, new in PAIRS]
    if old_counts == [1] * len(PAIRS) and new_counts == [0] * len(PAIRS):
        return "donor"
    if old_counts == [0] * len(PAIRS) and new_counts == [1] * len(PAIRS):
        return "adapted"

    details = ", ".join(
        f"{label}: old={old_count}, new={new_count}"
        for (label, _, _), old_count, new_count in zip(PAIRS, old_counts, new_counts)
    )
    raise AssertionError(f"partial or drifted PlayerbotAI C++14 slice: {details}")


def verify_adapted(text: str) -> None:
    code = strip_cpp_non_code(text)
    for marker in TARGETED_MARKERS:
        if marker in code:
            raise AssertionError(f"targeted PlayerbotAI post-C++14 marker remains after adaptation: {marker}")

    required = (
        MAYBE_UNUSED_NEW,
        STARTS_WITH_NEW,
        "Channel* channel = channelEntry.second;",
        "if (channel->GetChannelId() == chanId)",
        STRING_VIEW_NEW,
        SHARP_SIZE_NEW,
        WEIGHT_SIZE_NEW,
        QUEST_SIZE_NEW,
    )
    for fragment in required:
        if fragment not in text:
            raise AssertionError(f"PlayerbotAI adaptation lost required behavior fragment: {fragment}")


def adapt_text(text: str) -> tuple[str, bool]:
    state = classify_state(text)
    if state == "adapted":
        verify_adapted(text)
        return text, False

    adapted = text
    for _, old, new in PAIRS:
        adapted = adapted.replace(old, new, 1)

    if classify_state(adapted) != "adapted":
        raise AssertionError("PlayerbotAI C++14 adaptation did not reach the expected complete state")
    verify_adapted(adapted)
    return adapted, True


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true", help="write the reviewed transform to src/Bot/PlayerbotAI.cpp")
    parser.add_argument(
        "--require-adapted",
        action="store_true",
        help="fail unless src/Bot/PlayerbotAI.cpp already contains the fully adapted slice",
    )
    args = parser.parse_args()

    if not TARGET.is_file():
        raise AssertionError(f"missing donor source: {TARGET.relative_to(MODULE_ROOT)}")

    original = TARGET.read_text(encoding="utf-8")
    state = classify_state(original)
    if args.require_adapted and state != "adapted":
        raise AssertionError("PlayerbotAI reviewed C++14 source rewrite is required but donor state is present")

    adapted, changed = adapt_text(original)
    if args.write and changed:
        TARGET.write_text(adapted, encoding="utf-8")
        print("APPLIED: PlayerbotAI reviewed C++14 adaptation slice")
    elif changed:
        print("PASS: PlayerbotAI reviewed C++14 adaptation is applicable without source drift")
        print("PENDING: rerun with --write to apply the source rewrite")
    else:
        print("PASS: PlayerbotAI reviewed C++14 adaptation is already applied")

    if args.require_adapted:
        print("PASS: CI requires and observes the fully adapted PlayerbotAI source state")
    print("PASS: targeted post-C++14 markers are absent from the adapted result")
    print("PASS: toxic-link prefix, channel selection, aura-name, and loop-bound intent remain represented")
    print("NOTE: source adaptation evidence only; no runtime gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
