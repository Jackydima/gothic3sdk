#pragma once

#include "utility.h"

#include <g3sdk/Script.h>
#include <g3sdk/util/Hook.h>
#include <g3sdk/util/Logging.h>
#include <g3sdk/util/Memory.h>
#include <g3sdk/util/ScriptUtil.h>

gEAction GE_STDCALL AssessHit(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                              GEU32 a_iArgs);
