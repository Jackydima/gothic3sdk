add_library(Script_UI SHARED)

target_link_libraries(Script_UI PRIVATE g3sdk::SDK)

target_gothic_script_options(Script_UI)

target_tree_sources(Script_UI
  PRIVATE
    Script_UI.h
    Script_UI.cpp
    EXP_GUI.h
    EXP_GUI.cpp
)
