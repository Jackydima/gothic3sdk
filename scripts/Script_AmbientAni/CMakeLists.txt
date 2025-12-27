add_library(Script_AmbientAni SHARED)

target_link_libraries(Script_AmbientAni PRIVATE g3sdk::SDK)

target_gothic_script_options(Script_AmbientAni)

target_tree_sources(Script_AmbientAni
  PRIVATE
    Script_AmbientAni.h
    Script_AmbientAni.cpp
    utils.h
    utils.cpp
)
