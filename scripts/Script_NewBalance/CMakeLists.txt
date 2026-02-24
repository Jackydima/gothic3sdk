add_library(Script_NewBalance SHARED)

target_link_libraries(Script_NewBalance PRIVATE g3sdk::SDK)

target_gothic_script_options(Script_NewBalance)

target_tree_sources(Script_NewBalance
  PRIVATE
    CallHook.cpp
    CodePatch.cpp
    FunctionHook.cpp
    ge_DamageReceiver_ext.cpp
    MyGUI.cpp
    Script_NewBalance.cpp
    SharedConfig.cpp
    utility.cpp
    CallHook.h
    CodePatch.h
    FunctionHook.h
    ge_DamageReceiver_ext.h
    MyGUI.h
    Script_NewBalance.h
    SharedConfig.h
    utility.cpp
    utility.h
)