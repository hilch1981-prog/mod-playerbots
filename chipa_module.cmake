# Chipa PlayerBot V2 module manifest
#
# This file is consumed by MOP_V2_Repack/modules/CMakeLists.txt.
# Only sources explicitly listed here are compiled into the Chipa runtime.
# This prevents the untouched AzerothCore upstream tree from being compiled
# accidentally before each component has been adapted and verified for MoP 5.4.8.

set(CHIPA_MODULE_NAME "mod-playerbots")
set(CHIPA_MODULE_LOADER "Addmod_playerbotsScripts")

set(CHIPA_MODULE_SOURCES
  "${CMAKE_CURRENT_LIST_DIR}/src/chipa/ModuleBootstrap.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/chipa/PlayerLifecycleBridge.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/chipa/PlayerUpdateBridge.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/chipa/PlayerUpdateAdapter.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/chipa/PlayerUpdateScript.cpp"
)

set(CHIPA_MODULE_INCLUDE_DIRS
  "${CMAKE_CURRENT_LIST_DIR}/src/chipa"
)

# Compile-only donor closure probe. This is deliberately OFF by default and
# does not activate DonorPlayerbotBackend.cpp or change runtime behavior.
# CI may enable it to ask the real Chipa/SkyFire compiler whether the reviewed
# scheduler closure is source-compatible before those units enter the
# production manifest. Link/runtime evidence is still required later.
option(CHIPA_PLAYERBOT_DONOR_COMPILE_PROBE
  "Compile the reviewed PlayerBot donor closure probe without activating it"
  OFF)

if(CHIPA_PLAYERBOT_DONOR_COMPILE_PROBE)
  list(APPEND CHIPA_MODULE_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/src/Bot/Engine/PlayerbotAIBase.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/Bot/Debug/PerfMonitor.cpp"
  )
  list(APPEND CHIPA_MODULE_INCLUDE_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/src"
    "${CMAKE_CURRENT_LIST_DIR}/src/Bot"
    "${CMAKE_CURRENT_LIST_DIR}/src/Bot/Engine"
    "${CMAKE_CURRENT_LIST_DIR}/src/Bot/Debug"
  )
  message(STATUS "  + mod-playerbots donor compile probe: PlayerbotAIBase.cpp + PerfMonitor.cpp")
endif()
