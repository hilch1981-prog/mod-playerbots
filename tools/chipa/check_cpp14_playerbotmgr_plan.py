#!/usr/bin/env python3
"""Validate the first semantics-preserving C++14 adaptation slice for PlayerbotMgr.

This checker deliberately does not rewrite the donor source. It proves that the
three selected post-C++14 constructs still match the reviewed donor text and
that their C++14 replacements remove exactly those blockers without changing
the surrounding control-flow intent. Applying the source rewrite is a separate,
reviewable commit after this plan is green.
"""

from pathlib import Path
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


def require_exactly_once(text: str, fragment: str, label: str) -> None:
    count = text.count(fragment)
    if count != 1:
        raise AssertionError(f"{label}: expected reviewed fragment exactly once, found {count}")


def count_marker(text: str, marker: str) -> int:
    return strip_cpp_non_code(text).count(marker)


def main() -> int:
    if not TARGET.is_file():
        raise AssertionError(f"missing donor source: {TARGET.relative_to(MODULE_ROOT)}")

    original = TARGET.read_text(encoding="utf-8")

    require_exactly_once(original, CONTAINS_OLD, "contains adaptation")
    require_exactly_once(original, LOADING_LOOP_OLD, "structured-binding adaptation")
    require_exactly_once(original, DEAD_SESSION_OLD, "maybe_unused dead-session adaptation")

    preview = original.replace(CONTAINS_OLD, CONTAINS_NEW, 1)
    preview = preview.replace(LOADING_LOOP_OLD, LOADING_LOOP_NEW, 1)
    preview = preview.replace(DEAD_SESSION_OLD, DEAD_SESSION_NEW, 1)

    expected_deltas = {
        ".contains(": (1, 0),
        "for (auto const& [": (1, 0),
        "[[maybe_unused]]": (1, 0),
    }
    for marker, expected in expected_deltas.items():
        actual = (count_marker(original, marker), count_marker(preview, marker))
        if actual != expected:
            raise AssertionError(
                f"unexpected blocker delta for {marker!r}: expected {expected}, got {actual}"
            )

    # Pin the behavior that matters around the structured-binding rewrite.
    if "if (loadingEntry.second == masterAccountId)" not in preview:
        raise AssertionError("loading-account comparison was not preserved in the C++14 preview")
    if "++loadingForMaster;" not in preview:
        raise AssertionError("loading counter increment was lost in the C++14 preview")

    # The removed variables feed only the donor's already-commented timed-logout
    # sketch. Active instant-logout behavior must remain present in the preview.
    if "botWorldSessionPtr->LogoutPlayer(true);" not in preview:
        raise AssertionError("active instant-logout path was lost in the C++14 preview")
    if "delete botWorldSessionPtr;" not in preview:
        raise AssertionError("active WorldSession cleanup was lost in the C++14 preview")

    print("PASS: reviewed PlayerbotMgr C++14 adaptation fragments still match exactly")
    print("PASS: preview removes contains/structured-binding/maybe_unused blockers 1 -> 0")
    print("PASS: loading-count and active instant-logout semantics remain represented")
    print("PENDING: preview only; donor source is not rewritten and no gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
