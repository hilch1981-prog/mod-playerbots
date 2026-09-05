#!/usr/bin/env python3
"""Static contract checks for the Chipa MoP 5.4.8 G2 player-update bridge.

These checks intentionally prove only architecture/wiring invariants. They do not
claim runtime behavior or promote G1/G2 gates.
"""

from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]


def read(relative: str) -> str:
    path = ROOT / relative
    if not path.is_file():
        raise AssertionError(f"missing required file: {relative}")
    return path.read_text(encoding="utf-8")


def require(text: str, token: str, where: str) -> None:
    if token not in text:
        raise AssertionError(f"{where}: missing required token: {token}")


def forbid(text: str, token: str, where: str) -> None:
    if token in text:
        raise AssertionError(f"{where}: forbidden PlayerBot/runtime dependency leaked in: {token}")


def require_order(text: str, tokens: tuple[str, ...], where: str) -> None:
    positions = []
    for token in tokens:
        require(text, token, where)
        positions.append(text.index(token))
    if positions != sorted(positions):
        raise AssertionError(f"{where}: required ordering violated: {' -> '.join(tokens)}")


def main() -> int:
    manifest = read("chipa_module.cmake")
    bridge_h = read("src/chipa/PlayerUpdateBridge.h")
    bridge_cpp = read("src/chipa/PlayerUpdateBridge.cpp")
    adapter_h = read("src/chipa/PlayerUpdateAdapter.h")
    adapter_cpp = read("src/chipa/PlayerUpdateAdapter.cpp")
    script = read("src/chipa/PlayerUpdateScript.cpp")
    bootstrap = read("src/chipa/ModuleBootstrap.cpp")

    for source in (
        "src/chipa/ModuleBootstrap.cpp",
        "src/chipa/PlayerUpdateBridge.cpp",
        "src/chipa/PlayerUpdateAdapter.cpp",
        "src/chipa/PlayerUpdateScript.cpp",
    ):
        require(manifest, source, "chipa_module.cmake")

    require(script, "void OnUpdate(Player* player, uint32 diff) override", "PlayerUpdateScript.cpp")
    require(
        script,
        "chipa::playerbots::DispatchPlayerUpdate(player, static_cast<std::uint32_t>(diff));",
        "PlayerUpdateScript.cpp",
    )
    require(bootstrap, "void AddChipaPlayerbotUpdateScript();", "ModuleBootstrap.cpp")
    require(bootstrap, "void RegisterPlayerUpdateAdapter();", "ModuleBootstrap.cpp")
    require(bootstrap, "void Addmod_playerbotsScripts()", "ModuleBootstrap.cpp")
    bootstrap_body = bootstrap.split("void Addmod_playerbotsScripts()", 1)[1]
    require_order(
        bootstrap_body,
        (
            "chipa::playerbots::RegisterPlayerUpdateAdapter();",
            "AddChipaPlayerbotUpdateScript();",
        ),
        "Addmod_playerbotsScripts body",
    )

    if not (
        "typedef void (*PlayerUpdateCallback)" in bridge_h
        or "using PlayerUpdateCallback" in bridge_h
    ):
        raise AssertionError("PlayerUpdateBridge.h: PlayerUpdateCallback declaration missing")
    require(bridge_h, "SetPlayerUpdateCallback", "PlayerUpdateBridge.h")
    require(bridge_h, "DispatchPlayerUpdate", "PlayerUpdateBridge.h")
    require(bridge_cpp, "if (!player)", "PlayerUpdateBridge.cpp")
    require(bridge_cpp, "if (callback)", "PlayerUpdateBridge.cpp")

    for token in (
        "IsManagedPlayerCallback",
        "PlayerAiUpdateCallback",
        "PlayerMgrUpdateCallback",
        "ConfigurePlayerUpdateBackend",
        "ResetPlayerUpdateBackend",
        "RegisterPlayerUpdateAdapter",
    ):
        require(adapter_h, token, "PlayerUpdateAdapter.h")

    require(adapter_cpp, "SetPlayerUpdateCallback(&HandlePlayerUpdate);", "PlayerUpdateAdapter.cpp")
    require_order(
        adapter_cpp,
        (
            "if (!isManagedPlayer || !isManagedPlayer(player))",
            "updateAI(player, diff);",
            "updateManager(player, diff);",
        ),
        "PlayerUpdateAdapter.cpp",
    )
    forbid(adapter_cpp, "UpdateAIInternal", "PlayerUpdateAdapter.cpp")

    # Core-facing bridge and generic module adapter must stay free of concrete
    # PlayerBot/session/database implementation dependencies. A later donor-
    # specific backend may include those headers in a separate adapted TU.
    generic_boundary = bridge_h + "\n" + bridge_cpp + "\n" + adapter_h + "\n" + adapter_cpp
    for forbidden in (
        "PlayerbotAI.h",
        "PlayerbotMgr.h",
        "RandomPlayerbotMgr.h",
        "WorldSession.h",
        "DatabaseEnv.h",
    ):
        forbid(generic_boundary, forbidden, "generic bridge/adapter")

    print("PASS: Chipa G2 bridge static contract")
    print("NOTE: static contract only; no runtime gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
