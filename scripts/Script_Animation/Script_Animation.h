#pragma once

#include "Script.h"
#include "utils.h"

#include <g3sdk/util/Hook.h>
#include <g3sdk/util/Logging.h>
#include <g3sdk/util/Memory.h>
#include <g3sdk/util/Module.h>
#include <g3sdk/util/Util.h>

LPVOID GetMotionDataEntity_Ext(LPVOID p_ThisObject, gEAniState p_aniState, gEUseType p_UseTypeLeft,
                               gEUseType p_UseTypeRight, gEPose p_Pose, gEAction p_Action, gEPhase p_Phase,
                               gEDirection p_Direction, GEInt p_Variation, gEAmbientAction p_AmbientAction);

LPVOID Pre_GetMotionDataEntity(gEAniState p_aniState, gEUseType p_UseTypeLeft, gEUseType p_UseTypeRight, gEPose p_Pose,
                               gEAction p_Action, gEPhase p_Phase, gEDirection p_Direction, GEInt p_Variation,
                               gEAmbientAction p_AmbientAction);

LPVOID Call_GetMotionDataEntity(LPVOID CharacterAnimation, LPVOID AniObjectPtr,
                                gEAniState p_aniState, gEUseType p_UseTypeLeft, gEUseType p_UseTypeRight, gEPose p_Pose,
                                gEAction p_Action, gEPhase p_Phase, gEDirection p_Direction, GEInt p_Variation,
                                gEAmbientAction p_AmbientAction);


gSScriptInit &GetScriptInit();
