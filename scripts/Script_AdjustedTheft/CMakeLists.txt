add_library(Script_AdjustedTheft SHARED)

target_link_libraries(Script_AdjustedTheft PRIVATE g3sdk::SDK)

target_gothic_script_options(Script_AdjustedTheft)

target_tree_sources(Script_AdjustedTheft
  PRIVATE
    Script_AdjustedTheft.cpp
    Script_AdjustedTheft.h
)