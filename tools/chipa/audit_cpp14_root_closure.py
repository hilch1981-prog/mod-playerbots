#!/usr/bin/env python3
"""Inventory C++17/20 donor syntax that cannot enter the Chipa C++14 build.

This is an activation guard, not a compile PASS. The Chipa/SkyFire runtime
explicitly builds GCC sources with -std=c++14. The PlayerBot donor has evolved
past that baseline in a few root-closure files, so those constructs must be
adapted before the concrete donor backend is allowed into chipa_module.cmake.

The known blocker set is also a ratchet: counts may decrease as compatibility
work lands, but new post-C++14 constructs or count increases fail CI.
"""

from pathlib import Path
import argparse
import re
import sys

MODULE_ROOT = Path(__file__).resolve().parents[2]

DONOR_ROOTS = (
    "src/Script/Playerbots.cpp",
    "src/Bot/PlayerbotAI.cpp",
    "src/Bot/Engine/PlayerbotAIBase.cpp",
    "src/Bot/PlayerbotMgr.cpp",
)

# These are deliberately narrow, syntax-level markers for language/library
# features newer than the target's explicit C++14 baseline. They are not meant
# to prove the whole donor closure is compatible.
POST_CPP14_PATTERNS = (
    ("unordered/container contains()", re.compile(r"\.(?:contains)\s*\(")),
    ("std::string starts_with()/ends_with()", re.compile(r"\.(?:starts_with|ends_with)\s*\(")),
    ("structured binding", re.compile(r"\b(?:auto|const\s+auto|auto\s+const)\s*(?:&|&&)?\s*\[[A-Za-z_]\w*(?:\s*,\s*[A-Za-z_]\w*)+\]")),
    ("if constexpr", re.compile(r"\bif\s+constexpr\s*\(")),
    ("C++17 maybe_unused attribute", re.compile(r"\[\[\s*maybe_unused\s*\]\]")),
    ("std::optional", re.compile(r"\bstd::optional\b")),
    ("std::variant", re.compile(r"\bstd::variant\b")),
    ("std::any", re.compile(r"\bstd::any\b")),
    ("std::string_view", re.compile(r"\bstd::string_view\b")),
    ("std::filesystem", re.compile(r"\bstd::filesystem\b")),
    ("std::span", re.compile(r"\bstd::span\b")),
    ("std::ranges", re.compile(r"\bstd::ranges\b")),
    ("std::erase_if", re.compile(r"\bstd::erase_if\b")),
)

# Baseline captured before C++14 adaptation starts. This is intentionally a
# ceiling, not an expected exact count: every reduction is allowed, while any
# increase or newly introduced marker fails the contract.
BLOCKER_CEILING = {
    ("src/Bot/PlayerbotAI.cpp", "std::string starts_with()/ends_with()"): 1,
    ("src/Bot/PlayerbotMgr.cpp", "unordered/container contains()"): 1,
    ("src/Bot/PlayerbotMgr.cpp", "std::string starts_with()/ends_with()"): 1,
    ("src/Bot/PlayerbotMgr.cpp", "structured binding"): 1,
    ("src/Bot/PlayerbotMgr.cpp", "C++17 maybe_unused attribute"): 1,
}


def read(root: Path, relative: str) -> str:
    path = root / relative
    if not path.is_file():
        raise AssertionError(f"missing required file: {relative}")
    return path.read_text(encoding="utf-8")


def findings_for(relative: str, text: str) -> list[tuple[str, str, int]]:
    findings: list[tuple[str, str, int]] = []
    for label, pattern in POST_CPP14_PATTERNS:
        count = len(pattern.findall(text))
        if count:
            findings.append((relative, label, count))
    return findings


def verify_ratchet(findings: list[tuple[str, str, int]]) -> None:
    for relative, marker, count in findings:
        ceiling = BLOCKER_CEILING.get((relative, marker), 0)
        if count > ceiling:
            raise AssertionError(
                f"C++14 blocker regression: {relative}: {marker} x{count} exceeds ceiling x{ceiling}"
            )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("runtime_root", type=Path)
    args = parser.parse_args()
    runtime_root = args.runtime_root.resolve()

    gcc_settings = read(runtime_root, "cmake/compiler/gcc/settings.cmake")
    if '-std=c++14' not in gcc_settings:
        raise AssertionError(
            "live Chipa runtime no longer advertises -std=c++14 in GCC settings; "
            "re-evaluate this compatibility guard before changing it"
        )

    manifest = read(MODULE_ROOT, "chipa_module.cmake")
    bootstrap = read(MODULE_ROOT, "src/chipa/ModuleBootstrap.cpp")

    findings: list[tuple[str, str, int]] = []
    for relative in DONOR_ROOTS:
        findings.extend(findings_for(relative, read(MODULE_ROOT, relative)))

    verify_ratchet(findings)

    if findings:
        if "src/chipa/DonorPlayerbotBackend.cpp" in manifest:
            raise AssertionError(
                "post-C++14 blockers remain but donor backend is active in chipa_module.cmake"
            )
        if "ConfigureDonorPlayerUpdateBackend();" in bootstrap:
            raise AssertionError(
                "post-C++14 blockers remain but donor backend is active in ModuleBootstrap.cpp"
            )

    print("PASS: live Chipa GCC build baseline is explicitly -std=c++14")
    print("PASS: audited post-C++14 blocker counts did not increase")
    if findings:
        print("PENDING: post-C++14 donor syntax still requires C++14 adaptation:")
        for relative, marker, count in findings:
            print(f"  {relative}: {marker} x{count}")
        print("PASS: donor backend remains inactive while C++14 blockers exist")
    else:
        print("PASS: no audited post-C++14 syntax markers remain in root closure")
    print("NOTE: syntax inventory/activation guard only; no runtime gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
