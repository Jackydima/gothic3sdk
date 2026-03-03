#include "AIFunctions.h"

void ClearInputEntry(Entity a_Entity)
{
    using mFClearInputEntry = void(GE_STDCALL *)(Entity p_selfEntity);
    static mFClearInputEntry s_fClearInputEntry = force_cast<mFClearInputEntry>(RVA_ScriptGame(0x79a0));

    return s_fClearInputEntry(a_Entity);
}

ME_DEFINE_AND_REGISTER_SCRIPT_AI_FUNCTION(_AI_EvadeBackward)
{
    // Setup
    INIT_SCRIPT_STATE();
    gSArgsFor__AI_EvadeBackward param = *reinterpret_cast<gSArgsFor__AI_EvadeBackward *>(
        a_rRunTimeStack.GetAt(a_rRunTimeStack.GetCount() - 1).m_pArguments);

    // Execution in seperated Blocks!
    BREAK_BLOCK
    {
        ClearInputEntry(param.m_Self);

        if (GetScriptAdmin().CallScriptFromScript("GetStaminaPoints", &param.m_Self, &None) <= 15)
        {
            return GETrue;
        }

        param.m_Self.Routine.AccessProperty<PSRoutine::PropertyAction>() = gEAction_Evade;
        param.m_Self.Routine.AccessProperty<PSRoutine::PropertyAniState>() = gEAniState_Stand;

        

        // Consume Staminapoints
        GetScriptAdmin().CallScriptFromScript("AddStaminaPoints", &param.m_Self, &None, -15);

        // param.m_Self.Routine.SetLocalCallback("OnEvade");

        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Raise", 1.0f);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {
        // Better decide hits in OnLogicalDamage than removing collision group!
        // param.m_Self.SetCollisionGroup(eECollisionGroup_Ghost);
        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Hit", 1.0f);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {
        // Better decide hits in OnLogicalDamage than removing collision group!
        /*if (param.m_Self.IsPlayer())
        {
            param.m_Self.SetCollisionGroup(eECollisionGroup_Player);
        }
        else
        {
            param.m_Self.SetCollisionGroup(eECollisionGroup_NPC);
        }*/
        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Recover", 1.0f);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK

    return GETrue;
}

ME_DEFINE_AND_REGISTER_SCRIPT_AI_FUNCTION(_AI_EvadeRight)
{
    // Setup
    INIT_SCRIPT_STATE();
    gSArgsFor__AI_EvadeBackward param = *reinterpret_cast<gSArgsFor__AI_EvadeBackward *>(
        a_rRunTimeStack.GetAt(a_rRunTimeStack.GetCount() - 1).m_pArguments);

    // Execution in seperated Blocks!
    BREAK_BLOCK
    {
        ClearInputEntry(param.m_Self);

        if (GetScriptAdmin().CallScriptFromScript("GetStaminaPoints", &param.m_Self, &None) <= 15)
        {
            return GETrue;
        }

        param.m_Self.Routine.AccessProperty<PSRoutine::PropertyAction>() = gEAction_Evade;
        param.m_Self.Routine.AccessProperty<PSRoutine::PropertyAniState>() = gEAniState_Stand;

        // Consume Staminapoints
        GetScriptAdmin().CallScriptFromScript("AddStaminaPoints", &param.m_Self, &None, -15);

        // param.m_Self.Routine.SetLocalCallback("OnEvade");

        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Right_Raise", 1.0f);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {
        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Right_Hit", 1.0f);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {
        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Right_Recover", 1.0f);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK

    return GETrue;
}

ME_DEFINE_AND_REGISTER_SCRIPT_AI_FUNCTION(_AI_EvadeLeft)
{
    // Setup
    INIT_SCRIPT_STATE();
    gSArgsFor__AI_EvadeBackward param = *reinterpret_cast<gSArgsFor__AI_EvadeBackward *>(
        a_rRunTimeStack.GetAt(a_rRunTimeStack.GetCount() - 1).m_pArguments);

    // Execution in seperated Blocks!
    BREAK_BLOCK
    {
        ClearInputEntry(param.m_Self);

        if (GetScriptAdmin().CallScriptFromScript("GetStaminaPoints", &param.m_Self, &None) <= 15)
        {
            return GETrue;
        }

        param.m_Self.Routine.AccessProperty<PSRoutine::PropertyAction>() = gEAction_Evade;
        param.m_Self.Routine.AccessProperty<PSRoutine::PropertyAniState>() = gEAniState_Stand;

        // Consume Staminapoints
        GetScriptAdmin().CallScriptFromScript("AddStaminaPoints", &param.m_Self, &None, -15);

        // param.m_Self.Routine.SetLocalCallback("OnEvade");

        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Left_Raise", 1.0f);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {
        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Left_Hit", 1.0f);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {
        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Left_Recover", 1.0f);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK

    return GETrue;
}
