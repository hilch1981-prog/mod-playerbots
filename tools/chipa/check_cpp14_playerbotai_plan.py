#!/usr/bin/env python3
"""Validate the reviewed PlayerbotAI C++14 adaptation slice/state.

The contract accepts exactly two coherent states: the original reviewed donor
fragments, where a deterministic preview must adapt cleanly, or the fully
adapted source. Partial rewrites and drift fail. This is source compatibility
evidence only and must not promote G1 or G2.
"""

from pathlib import Path
import sys

from adapt_cpp14_playerbotai import TARGET, adapt_text, classify_state, verify_adapted

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
            raise AssertionError("reviewed PlayerbotAI donor state did not produce the expected C++14 preview")
        print("PASS: reviewed PlayerbotAI donor fragments still match exactly")
        print("PASS: deterministic preview removes starts_with/structured-binding/maybe_unused/string_view/std::size blockers")
        print("PENDING: reviewed PlayerbotAI source rewrite is not yet applied")
    elif state == "adapted":
        if changed:
            raise AssertionError("adapted PlayerbotAI source unexpectedly requested another rewrite")
        print("PASS: reviewed PlayerbotAI C++14 source rewrite is fully applied")
        print("PASS: targeted PlayerbotAI post-C++14 blockers remain removed")
    else:
        raise AssertionError(f"unexpected PlayerbotAI adaptation state: {state}")

    print("PASS: toxic-link prefix, channel selection, aura-name, and loop-bound intent remain represented")
    print("NOTE: source compatibility evidence only; no runtime gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
