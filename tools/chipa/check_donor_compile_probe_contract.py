#!/usr/bin/env python3
"""Guard the first Chipa donor source-closure compile probe.

The probe must stay opt-in and compile-only. It is allowed to expose the
reviewed scheduler unit to the real SkyFire compiler, but it must not activate
the donor backend or silently pull PlayerbotMgr/PlayerbotAI production roots
into the normal module manifest. Passing this contract is not G2 PASS.
"""

from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
MANIFEST = ROOT / "chipa_module.cmake"


def require(text: str, fragment: str, label: str) -> None:
    if fragment not in text:
        raise AssertionError(f"missing {label}: {fragment}")


def strip_cmake_comments(text: str) -> str:
    """Remove full-line CMake comments before activation-boundary checks.

    Human-readable comments intentionally name forbidden production files such
    as DonorPlayerbotBackend.cpp. Those names are evidence/documentation, not
    CMake activation, so the guard must inspect executable manifest text only.
    """
    return "\n".join(
        line for line in text.splitlines()
        if not line.lstrip().startswith("#")
    )


def main() -> int:
    text = MANIFEST.read_text(encoding="utf-8")
    code = strip_cmake_comments(text)

    require(code, "option(CHIPA_PLAYERBOT_DONOR_COMPILE_PROBE", "opt-in probe option")
    require(code, '  OFF)', "default-OFF probe state")
    require(code, "if(CHIPA_PLAYERBOT_DONOR_COMPILE_PROBE)", "probe guard")
    require(
        code,
        '"${CMAKE_CURRENT_LIST_DIR}/src/Bot/Engine/PlayerbotAIBase.cpp"',
        "reviewed scheduler probe source",
    )
    require(
        code,
        'message(STATUS "  + mod-playerbots donor compile probe: PlayerbotAIBase.cpp")',
        "probe discovery evidence marker",
    )

    guarded = code.split("if(CHIPA_PLAYERBOT_DONOR_COMPILE_PROBE)", 1)[1]
    guarded = guarded.split("endif()", 1)[0]
    if "DonorPlayerbotBackend.cpp" in guarded:
        raise AssertionError("compile probe must not activate DonorPlayerbotBackend.cpp")
    for forbidden in ("src/Bot/PlayerbotAI.cpp", "src/Bot/PlayerbotMgr.cpp", "src/Script/Playerbots.cpp"):
        if forbidden in guarded:
            raise AssertionError(f"first scheduler probe unexpectedly widened closure: {forbidden}")

    normal_prefix = code.split("if(CHIPA_PLAYERBOT_DONOR_COMPILE_PROBE)", 1)[0]
    if "src/Bot/Engine/PlayerbotAIBase.cpp" in normal_prefix:
        raise AssertionError("PlayerbotAIBase.cpp escaped the opt-in probe guard")
    if "DonorPlayerbotBackend.cpp" in normal_prefix:
        raise AssertionError("donor backend is active in the normal manifest")

    print("PASS: donor compile probe is opt-in and default OFF")
    print("PASS: first closure unit is exactly PlayerbotAIBase.cpp")
    print("PASS: PlayerbotAI/PlayerbotMgr/Playerbots roots and donor backend remain inactive")
    print("PASS: activation checks ignore documentation-only CMake comments")
    print("NOTE: compile-probe contract only; no runtime gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (AssertionError, OSError) as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
