add_library(Script_RestoreWeapons SHARED)

target_link_libraries(Script_RestoreWeapons PRIVATE g3sdk::SDK)

target_gothic_script_options(Script_RestoreWeapons)

target_tree_sources(Script_RestoreWeapons
  PRIVATE
    Script_RestoreWeapons.h
    Script_RestoreWeapons.cpp
    RestoreGui.h
    RestoreGui.cpp
)
