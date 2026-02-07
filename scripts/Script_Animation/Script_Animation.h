#pragma once

#include "Script.h"
#include "utils.h"

#include <g3sdk/util/Hook.h>
#include <g3sdk/util/Logging.h>
#include <g3sdk/util/Memory.h>
#include <g3sdk/util/Module.h>
#include <g3sdk/util/Util.h>

void LoadConfig();

void CustomizeAniString(Entity &p_entity, bCString &p_aniString);

/* LPVOID GetMotionDataEntity_Ext(LPVOID p_ThisObject, gEAniState p_aniState, gEUseType p_UseTypeLeft,
                               gEUseType p_UseTypeRight, gEPose p_Pose, gEAction p_Action, gEPhase p_Phase,
                               gEDirection p_Direction, GEInt p_Variation, gEAmbientAction p_AmbientAction);

LPVOID Pre_GetMotionDataEntity(gEAniState p_aniState, gEUseType p_UseTypeLeft, gEUseType p_UseTypeRight, gEPose p_Pose,
                               gEAction p_Action, gEPhase p_Phase, gEDirection p_Direction, GEInt p_Variation,
                               gEAmbientAction p_AmbientAction);
                               
                               
LPVOID Call_GetMotionDataEntity(LPVOID CharacterAnimation, LPVOID AniObjectPtr,
                                gEAniState p_aniState, gEUseType p_UseTypeLeft, gEUseType p_UseTypeRight, gEPose p_Pose,
                                gEAction p_Action, gEPhase p_Phase, gEDirection p_Direction, GEInt p_Variation,
                                gEAmbientAction p_AmbientAction);*/

void GetMotionDataEntityAniString(LPVOID p_characterAnimationPtr, bCString *p_animationString);

bCString GE_STDCALL GetAniEx(gEUseType p_UseTypeLeft, gEUseType p_UseTypeRight, gEAction p_Action, gEPhase p_Phase,
                            bCString &p_String, GEBool p_Bool);

void GE_STDCALL GetAniName(bCString &p_RetString, eCEntity *p_Entity, gEAction p_Action, bCString p_Str1,
                           bCString &p_Str2, GEBool p_Bool);

GEBool GE_STDCALL PSAnimation_GetSkeletonName(PSAnimation const &This, bCString &o_SkeletonName);

gSScriptInit &GetScriptInit();
