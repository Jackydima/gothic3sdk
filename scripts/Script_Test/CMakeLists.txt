add_library(Script_Test SHARED)

target_link_libraries(Script_Test PRIVATE g3sdk::SDK)

target_gothic_script_options(Script_Test)

target_tree_sources(Script_Test
  PRIVATE
  Script_Test.cpp
  Script_Test.h
)