add_library(Script_Animation SHARED)

target_link_libraries(Script_Animation PRIVATE g3sdk::SDK)

target_gothic_script_options(Script_Animation)

target_tree_sources(Script_Animation
  PRIVATE
    Script_Animation.h
    Script_Animation.cpp
    utils.h
    utils.cpp
    resource.h
    Script_Animation.rc
)
