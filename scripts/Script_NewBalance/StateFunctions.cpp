#include "StateFunctions.h"

#include "ScriptArgs.h"

ME_DEFINE_AND_REGISTER_SCRIPT_AI_STATE(NB_EvadeBackward)
{
    INIT_SCRIPT_STATE();
    BREAK_BLOCK
    {
        Entity target = SelfEntity.NPC.GetCurrentTarget();

        if (!Entity::GetCamera().IsInFirstPerson())
        {
            SelfEntity.SetAlignmentTarget(target);
            SelfEntity.CharacterControl.SetMovementConstraints(360.0f);
        }

        PUSH_STATE_AND_ARGS(_AI_EvadeBackward);
        args->m_Self = SelfEntity;
        args->m_Other = target;
        RUN_SCRIPT_FUNCTION();
    }
    BREAK_BLOCK
    {
        GetScriptAdmin().CallScriptFromScript("AssessAgressor", &SelfEntity, &TargetEntity);
    }
    return GETrue;
}

ME_DEFINE_AND_REGISTER_SCRIPT_AI_STATE(NB_EvadeRight)
{
    INIT_SCRIPT_STATE();
    BREAK_BLOCK
    {
        Entity target = SelfEntity.NPC.GetCurrentTarget();

        if (!Entity::GetCamera().IsInFirstPerson())
        {
            SelfEntity.SetAlignmentTarget(target);
            SelfEntity.CharacterControl.SetMovementConstraints(360.0f);
        }

        PUSH_STATE_AND_ARGS(_AI_EvadeRight);
        args->m_Self = SelfEntity;
        args->m_Other = target;
        RUN_SCRIPT_FUNCTION();
    }
    BREAK_BLOCK
    {
        GetScriptAdmin().CallScriptFromScript("AssessAgressor", &SelfEntity, &TargetEntity);
    }
    return GETrue;
}

ME_DEFINE_AND_REGISTER_SCRIPT_AI_STATE(NB_EvadeLeft)
{
    INIT_SCRIPT_STATE();
    BREAK_BLOCK
    {
        Entity target = SelfEntity.NPC.GetCurrentTarget();

        if (!Entity::GetCamera().IsInFirstPerson())
        {
            SelfEntity.SetAlignmentTarget(target);
            SelfEntity.CharacterControl.SetMovementConstraints(360.0f);
        }

        PUSH_STATE_AND_ARGS(_AI_EvadeLeft);
        args->m_Self = SelfEntity;
        args->m_Other = target;
        RUN_SCRIPT_FUNCTION();
    }
    BREAK_BLOCK
    {
        GetScriptAdmin().CallScriptFromScript("AssessAgressor", &SelfEntity, &TargetEntity);
    }
    return GETrue;
}
