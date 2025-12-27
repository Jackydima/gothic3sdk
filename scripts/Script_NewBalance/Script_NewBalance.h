#pragma once

#include <g3sdk/Script.h>

#include "FunctionHook.h"
#include "CallHook.h"
#include "CodePatch.h"
#include "ge_DamageReceiver_ext.h"

#include "MyGUI.h"
#include <iostream>
#include <g3sdk/util/Memory.h>
#include <g3sdk/util/Logging.h>
#include <g3sdk/util/Hook.h>
#include <g3sdk/util/ScriptUtil.h>

#include "utility.h"
#include "SharedConfig.h"

gSScriptInit & GetScriptInit();

GEInt CanFreezeAddition ( gCScriptProcessingUnit* a_pSPU , Entity* a_pSelfEntity , Entity* a_pOtherEntity , GEU32 a_iArgs );

static GEU32 getPerfectBlockLastTime ( bCString iD);

GEInt StaminaUpdateOnTickHelper ( Entity& p_entity , GEInt p_staminaValue );

void AddNewEffect ( );