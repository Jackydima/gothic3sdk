add_library(Script_RestoreWeapons SHARED)

target_link_libraries(Script_Script_RestoreWeaponsTest PRIVATE g3sdk::SDK)

target_gothic_script_options(Script_RestoreWeapons)

target_tree_sources(Script_RestoreWeapons
  PRIVATE
  Script_RestoreWeapons.cpp
  Script_RestoreWeapons.h
)