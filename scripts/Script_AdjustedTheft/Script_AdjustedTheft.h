#pragma once
#include <g3sdk/Script.h>
#include <g3sdk/util/Hook.h>
#include <g3sdk/util/Logging.h>
#include <g3sdk/util/Memory.h>
#include <g3sdk/util/Module.h>

#ifndef NDEBUG
#include <iostream>
#endif // !NDEBUG



extern "C" __declspec(dllexport) gSScriptInit const *GE_STDCALL ScriptInit(void);
