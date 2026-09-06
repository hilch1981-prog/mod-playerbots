#!/usr/bin/env python3
"""Guard the Chipa donor scheduler source-closure compile probe.

The probe must stay opt-in and compile-only. It may expose the reviewed base
scheduler implementation plus its performance-monitor implementation to the
real SkyFire compiler, but it must not activate the donor backend or silently
pull PlayerbotMgr/PlayerbotAI production roots into the normal module manifest.
Passing this contract is source-boundary evidence only and is not G2 PASS.
"""

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
MANIFEST = ROOT / "chipa_module.cmake"

PROBE_SOURCES = (
    "src/Bot/Engine/PlayerbotAIBase.cpp",
    "src/Bot/Debug/PerfMonitor.cpp",
)

FORBIDDEN_ROOTS = (
    "src/Bot/PlayerbotAI.cpp",
    "src/Bot/PlayerbotMgr.cpp",
    "src/Script/Playerbots.cpp",
    "src/chipa/DonorPlayerbotBackend.cpp",
)


def require(text: str, fragment: str, label: str) -> None:
    if fragment not in text:
        raise AssertionError(f"missing {label}: {fragment}")


def strip_cmake_comments(text: str) -> str:
    """Remove full-line CMake comments before activation-boundary checks."""
    return "\n".join(
        line for line in text.splitlines()
        if not line.lstrip().startswith("#")
    )


def source_paths(block: str) -> list[str]:
    return re.findall(r'\$\{CMAKE_CURRENT_LIST_DIR\}/([^"\n]+\.cpp)', block)


def main() -> int:
    text = MANIFEST.read_text(encoding="utf-8")
    code = strip_cmake_comments(text)

    require(code, "option(CHIPA_PLAYERBOT_DONOR_COMPILE_PROBE", "opt-in probe option")
    require(code, '  OFF)', "default-OFF probe state")
    require(code, "if(CHIPA_PLAYERBOT_DONOR_COMPILE_PROBE)", "probe guard")
    for source in PROBE_SOURCES:
        require(code, f'"${{CMAKE_CURRENT_LIST_DIR}}/{source}"', f"reviewed probe source {source}")
    require(
        code,
        'message(STATUS "  + mod-playerbots donor compile probe: PlayerbotAIBase.cpp + PerfMonitor.cpp")',
        "probe discovery evidence marker",
    )

    normal_prefix, guarded_and_after = code.split("if(CHIPA_PLAYERBOT_DONOR_COMPILE_PROBE)", 1)
    guarded = guarded_and_after.split("endif()", 1)[0]

    observed_probe_sources = tuple(source_paths(guarded))
    if observed_probe_sources != PROBE_SOURCES:
        raise AssertionError(
            "scheduler probe source set/order drifted: "
            f"expected={PROBE_SOURCES}, observed={observed_probe_sources}"
        )

    for forbidden in FORBIDDEN_ROOTS:
        if forbidden in guarded:
            raise AssertionError(f"scheduler compile probe unexpectedly widened closure: {forbidden}")

    for source in PROBE_SOURCES:
        if source in normal_prefix:
            raise AssertionError(f"probe source escaped the opt-in guard: {source}")
    for forbidden in FORBIDDEN_ROOTS:
        if forbidden in normal_prefix:
            raise AssertionError(f"forbidden donor production source is active in normal manifest: {forbidden}")

    print("PASS: donor compile probe is opt-in and default OFF")
    print("PASS: scheduler probe source set is exactly PlayerbotAIBase.cpp + PerfMonitor.cpp")
    print("PASS: PlayerbotAI/PlayerbotMgr/Playerbots roots and donor backend remain inactive")
    print("PASS: activation checks ignore documentation-only CMake comments")
    print("NOTE: compile-probe boundary evidence only; no runtime gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (AssertionError, OSError) as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
