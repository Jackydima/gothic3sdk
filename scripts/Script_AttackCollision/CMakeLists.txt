add_library(Script_AttackCollision SHARED)

target_link_libraries(Script_AttackCollision PRIVATE g3sdk::SDK)

target_gothic_script_options(Script_AttackCollision)

target_tree_sources(Script_AttackCollision
  PRIVATE
  Script_AttackCollision.cpp
  Script_AttackCollision.h
  config.h
  config.cpp
)