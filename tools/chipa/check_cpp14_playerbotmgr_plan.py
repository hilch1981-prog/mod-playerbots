#!/usr/bin/env python3
"""Validate the reviewed PlayerbotMgr C++14 adaptation slice.

The contract accepts exactly two coherent states: the original reviewed donor
fragments (where a deterministic preview must adapt cleanly), or the fully
adapted source. Partial rewrites and source drift fail. This remains a source
compatibility contract only and must not promote G1 or G2.
"""

from pathlib import Path
import sys

from adapt_cpp14_playerbotmgr import TARGET, adapt_text, classify_state, verify_adapted

MODULE_ROOT = Path(__file__).resolve().parents[2]


def main() -> int:
    if not TARGET.is_file():
        raise AssertionError(f"missing donor source: {TARGET.relative_to(MODULE_ROOT)}")

    original = TARGET.read_text(encoding="utf-8")
    state = classify_state(original)
    adapted, changed = adapt_text(original)
    verify_adapted(adapted)

    if state == "donor":
        if not changed:
            raise AssertionError("reviewed donor state did not produce the expected C++14 preview")
        print("PASS: reviewed PlayerbotMgr donor fragments still match exactly")
        print("PASS: deterministic preview removes all four audited PlayerbotMgr post-C++14 blockers")
        print("PENDING: reviewed source rewrite is not yet applied")
    elif state == "adapted":
        if changed:
            raise AssertionError("adapted PlayerbotMgr source unexpectedly requested another rewrite")
        print("PASS: reviewed PlayerbotMgr C++14 source rewrite is fully applied")
        print("PASS: all four audited PlayerbotMgr post-C++14 blockers remain removed")
    else:
        raise AssertionError(f"unexpected PlayerbotMgr adaptation state: {state}")

    print("PASS: loading-count, init= prefix gate, and active instant-logout semantics remain represented")
    print("NOTE: source compatibility evidence only; no runtime gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
