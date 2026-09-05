#!/usr/bin/env python3
"""Static contract checks for the Chipa MoP 5.4.8 G2 integration bridge.

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
    lifecycle_h = read("src/chipa/PlayerLifecycleBridge.h")
    lifecycle_cpp = read("src/chipa/PlayerLifecycleBridge.cpp")
    bridge_h = read("src/chipa/PlayerUpdateBridge.h")
    bridge_cpp = read("src/chipa/PlayerUpdateBridge.cpp")
    adapter_h = read("src/chipa/PlayerUpdateAdapter.h")
    adapter_cpp = read("src/chipa/PlayerUpdateAdapter.cpp")
    script = read("src/chipa/PlayerUpdateScript.cpp")
    bootstrap = read("src/chipa/ModuleBootstrap.cpp")
    donor_script = read("src/Script/Playerbots.cpp")

    for source in (
        "src/chipa/ModuleBootstrap.cpp",
        "src/chipa/PlayerLifecycleBridge.cpp",
        "src/chipa/PlayerUpdateBridge.cpp",
        "src/chipa/PlayerUpdateAdapter.cpp",
        "src/chipa/PlayerUpdateScript.cpp",
    ):
        require(manifest, source, "chipa_module.cmake")

    require(script, "void OnLogin(Player* player) override", "PlayerUpdateScript.cpp")
    require(script, "chipa::playerbots::DispatchPlayerLogin(player);", "PlayerUpdateScript.cpp")
    require(script, "void OnLogout(Player* player) override", "PlayerUpdateScript.cpp")
    require(script, "chipa::playerbots::DispatchPlayerLogout(player);", "PlayerUpdateScript.cpp")
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
        "typedef void (*PlayerLifecycleCallback)" in lifecycle_h
        or "using PlayerLifecycleCallback" in lifecycle_h
    ):
        raise AssertionError("PlayerLifecycleBridge.h: PlayerLifecycleCallback declaration missing")
    for token in (
        "SetPlayerLoginCallback",
        "SetPlayerLogoutCallback",
        "DispatchPlayerLogin",
        "DispatchPlayerLogout",
    ):
        require(lifecycle_h, token, "PlayerLifecycleBridge.h")
        require(lifecycle_cpp, token, "PlayerLifecycleBridge.cpp")
    require(lifecycle_cpp, "if (!player)", "PlayerLifecycleBridge.cpp")
    require(lifecycle_cpp, "if (callback)", "PlayerLifecycleBridge.cpp")

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
    require(adapter_cpp, "g_backendGeneration", "PlayerUpdateAdapter.cpp")
    require(adapter_cpp, "std::atomic_flag g_backendWriteLock = ATOMIC_FLAG_INIT;", "PlayerUpdateAdapter.cpp")
    require(adapter_cpp, "BackendSnapshot LoadBackendSnapshot()", "PlayerUpdateAdapter.cpp")
    require(adapter_cpp, "LockBackendWriter();", "PlayerUpdateAdapter.cpp")
    require(adapter_cpp, "BeginBackendWrite();", "PlayerUpdateAdapter.cpp")
    require(adapter_cpp, "EndBackendWrite();", "PlayerUpdateAdapter.cpp")
    require(adapter_cpp, "UnlockBackendWriter();", "PlayerUpdateAdapter.cpp")
    configure_body = adapter_cpp.split("void ConfigurePlayerUpdateBackend", 1)[1].split("void ResetPlayerUpdateBackend", 1)[0]
    require_order(
        configure_body,
        (
            "LockBackendWriter();",
            "BeginBackendWrite();",
            "g_isManagedPlayer.store",
            "g_updateAI.store",
            "g_updateManager.store",
            "EndBackendWrite();",
            "UnlockBackendWriter();",
        ),
        "ConfigurePlayerUpdateBackend",
    )
    reset_body = adapter_cpp.split("void ResetPlayerUpdateBackend", 1)[1].split("void RegisterPlayerUpdateAdapter", 1)[0]
    require_order(
        reset_body,
        (
            "LockBackendWriter();",
            "BeginBackendWrite();",
            "g_isManagedPlayer.store",
            "g_updateAI.store",
            "g_updateManager.store",
            "EndBackendWrite();",
            "UnlockBackendWriter();",
        ),
        "ResetPlayerUpdateBackend",
    )
    require_order(
        adapter_cpp.split("void HandlePlayerUpdate", 1)[1].split("}\n}", 1)[0],
        (
            "if (!player)",
            "BackendSnapshot const backend = LoadBackendSnapshot();",
            "if (!backend.isManagedPlayer || !backend.isManagedPlayer(player))",
            "backend.updateAI(player, diff);",
            "backend.updateManager(player, diff);",
        ),
        "HandlePlayerUpdate",
    )
    forbid(adapter_cpp, "UpdateAIInternal", "PlayerUpdateAdapter.cpp")

    # Lock the donor public scheduling contract independently of the generic
    # bridge. The future MoP compatibility TU may adapt ownership/lookups, but
    # it must preserve the donor's public AI -> manager update order and must
    # never shortcut into UpdateAIInternal().
    donor_marker = "void OnPlayerAfterUpdate(Player* player, uint32 diff) override"
    require(donor_script, donor_marker, "Playerbots.cpp")
    donor_hook = donor_script.split(donor_marker, 1)[1].split("bool OnPlayerCanUseChat", 1)[0]
    require_order(
        donor_hook,
        (
            "PlayerbotsMgr::instance().GetPlayerbotAI(player)",
            "botAI->UpdateAI(diff);",
            "GET_PLAYERBOT_MGR(player)",
            "playerbotMgr->UpdateAI(diff);",
        ),
        "PlayerbotsPlayerScript::OnPlayerAfterUpdate",
    )
    forbid(donor_hook, "UpdateAIInternal(", "PlayerbotsPlayerScript::OnPlayerAfterUpdate")

    # Core-facing bridges and generic module adapter must stay free of concrete
    # PlayerBot/session/database implementation dependencies. A later donor-
    # specific backend may include those headers in a separate adapted TU.
    generic_boundary = (
        lifecycle_h
        + "\n"
        + lifecycle_cpp
        + "\n"
        + bridge_h
        + "\n"
        + bridge_cpp
        + "\n"
        + adapter_h
        + "\n"
        + adapter_cpp
    )
    for forbidden in (
        "PlayerbotAI.h",
        "PlayerbotMgr.h",
        "RandomPlayerbotMgr.h",
        "WorldSession.h",
        "DatabaseEnv.h",
    ):
        forbid(generic_boundary, forbidden, "generic bridge/adapter")

    print("PASS: Chipa G2 integration bridge static contract")
    print("PASS: donor public scheduling contract AI -> manager")
    print("NOTE: static contract only; no runtime gate is promoted")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
