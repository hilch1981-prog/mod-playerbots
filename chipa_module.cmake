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
  "${CMAKE_CURRENT_LIST_DIR}/src/chipa/PlayerUpdateBridge.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/chipa/PlayerUpdateAdapter.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/chipa/PlayerUpdateScript.cpp"
)

set(CHIPA_MODULE_INCLUDE_DIRS
  "${CMAKE_CURRENT_LIST_DIR}/src/chipa"
)
