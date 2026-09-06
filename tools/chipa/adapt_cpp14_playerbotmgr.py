#!/usr/bin/env python3
"""Apply the reviewed C++14 adaptation slice to PlayerbotMgr.cpp.

The transform is intentionally narrow and all-or-nothing. It only rewrites the
four fragments already pinned by check_cpp14_playerbotmgr_plan.py. A partial or
drifted source state fails rather than guessing. Passing this tool is source
adaptation evidence only; it does not promote G1 or G2.
"""

from pathlib import Path
import argparse
import sys

from audit_cpp14_root_closure import strip_cpp_non_code

MODULE_ROOT = Path(__file__).resolve().parents[2]
TARGET = MODULE_ROOT / "src/Bot/PlayerbotMgr.cpp"

CONTAINS_OLD = "if (!botsBeingInitialized.contains(guid))"
CONTAINS_NEW = "if (botsBeingInitialized.find(guid) == botsBeingInitialized.end())"

LOADING_LOOP_OLD = """        for (auto const& [guid, acctId] : botLoading)
        {
            if (acctId == masterAccountId)
                ++loadingForMaster;
        }
"""
LOADING_LOOP_NEW = """        for (auto const& loadingEntry : botLoading)
        {
            if (loadingEntry.second == masterAccountId)
                ++loadingForMaster;
        }
"""

DEAD_SESSION_OLD = """        WorldSession* botWorldSessionPtr = bot->GetSession();
        [[maybe_unused]] WorldSession* masterWorldSessionPtr = nullptr;     // Remove [[maybe_unused]] tag if timed logout implemented.

        if (botWorldSessionPtr->isLogingOut())
            return;

        Player* master = botAI->GetMaster();
        if (master)
            masterWorldSessionPtr = master->GetSession();

"""
DEAD_SESSION_NEW = """        WorldSession* botWorldSessionPtr = bot->GetSession();

        if (botWorldSessionPtr->isLogingOut())
            return;

"""

STARTS_WITH_OLD = "else if (cmd.starts_with(\"init=\") && sscanf(cmd.c_str(), \"init=%d\", &gs) != -1)"
STARTS_WITH_NEW = "else if (cmd.compare(0, 5, \"init=\") == 0 && sscanf(cmd.c_str(), \"init=%d\", &gs) != -1)"

PAIRS = (
    ("contains adaptation", CONTAINS_OLD, CONTAINS_NEW),
    ("structured-binding adaptation", LOADING_LOOP_OLD, LOADING_LOOP_NEW),
    ("maybe_unused dead-session adaptation", DEAD_SESSION_OLD, DEAD_SESSION_NEW),
    ("starts_with adaptation", STARTS_WITH_OLD, STARTS_WITH_NEW),
)

TARGETED_MARKERS = (
    ".contains(",
    "for (auto const& [",
    "[[maybe_unused]]",
    ".starts_with(",
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
    raise AssertionError(f"partial or drifted PlayerbotMgr C++14 slice: {details}")


def verify_adapted(text: str) -> None:
    code = strip_cpp_non_code(text)
    for marker in TARGETED_MARKERS:
        if marker in code:
            raise AssertionError(f"targeted post-C++14 marker remains after adaptation: {marker}")

    if "if (loadingEntry.second == masterAccountId)" not in text or "++loadingForMaster;" not in text:
        raise AssertionError("loading-account counting semantics were not preserved")
    if "botWorldSessionPtr->LogoutPlayer(true);" not in text or "delete botWorldSessionPtr;" not in text:
        raise AssertionError("active instant-logout semantics were not preserved")
    if STARTS_WITH_NEW not in text:
        raise AssertionError("init= prefix gate was not preserved")


def adapt_text(text: str) -> tuple[str, bool]:
    state = classify_state(text)
    if state == "adapted":
        verify_adapted(text)
        return text, False

    adapted = text
    for _, old, new in PAIRS:
        adapted = adapted.replace(old, new, 1)

    if classify_state(adapted) != "adapted":
        raise AssertionError("PlayerbotMgr C++14 adaptation did not reach the expected complete state")
    verify_adapted(adapted)
    return adapted, True


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true", help="write the reviewed transform to src/Bot/PlayerbotMgr.cpp")
    args = parser.parse_args()

    if not TARGET.is_file():
        raise AssertionError(f"missing donor source: {TARGET.relative_to(MODULE_ROOT)}")

    original = TARGET.read_text(encoding="utf-8")
    adapted, changed = adapt_text(original)

    if args.write and changed:
        TARGET.write_text(adapted, encoding="utf-8")
        print("APPLIED: PlayerbotMgr reviewed C++14 adaptation slice")
    elif changed:
        print("PASS: PlayerbotMgr reviewed C++14 adaptation is applicable without source drift")
        print("PENDING: rerun with --write to apply the source rewrite")
    else:
        print("PASS: PlayerbotMgr reviewed C++14 adaptation is already applied")

    print("PASS: targeted post-C++14 markers are absent from the adapted result")
    print("PASS: loading-count, init= prefix gate, and instant-logout semantics remain represented")
    print("NOTE: source adaptation evidence only; no runtime gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
