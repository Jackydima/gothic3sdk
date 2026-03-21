#include "AIFunctions.h"

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
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Raise", NBConfig::fEvadeSpeed);

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
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Hit", NBConfig::fEvadeSpeed);

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
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Recover", NBConfig::fEvadeSpeed);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {}

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
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Right_Raise", NBConfig::fEvadeSpeed);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {
        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Right_Hit", NBConfig::fEvadeSpeed);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {
        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Right_Recover", NBConfig::fEvadeSpeed);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {}

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
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Left_Raise", NBConfig::fEvadeSpeed);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {
        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Left_Hit", NBConfig::fEvadeSpeed);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {
        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Evade, "Left_Recover", NBConfig::fEvadeSpeed);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {}

    return GETrue;
}

ME_DEFINE_AND_REGISTER_SCRIPT_AI_FUNCTION(_AI_ParryStumble)
{
    // Setup
    INIT_SCRIPT_STATE();
    gSArgsFor__AI_ParryStumble param = *reinterpret_cast<gSArgsFor__AI_ParryStumble *>(
        a_rRunTimeStack.GetAt(a_rRunTimeStack.GetCount() - 1).m_pArguments);

    // Execution in seperated Blocks!
    BREAK_BLOCK
    {
        gCScriptAdmin &ScriptAdmin = GetScriptAdmin();
        if (ScriptAdmin.CallScriptFromScript("IsHumanoid", &param.m_Self, &None))
        {
            ScriptAdmin.CallScriptFromScript("StartSayAargh", &param.m_Self, &None);
        }

        bCString sCurrentMomventAni = param.m_Self.NPC.GetCurrentMovementAni();

        param.m_Self.Routine.AccessProperty<PSRoutine::PropertyAction>() = gEAction_Stumble;
        param.m_Self.Routine.AccessProperty<PSRoutine::PropertyAniState>() = gEAniState_Stand;

        ClearInputEntry(param.m_Self);

        Entity attackerWeapon = param.m_Other.GetWeapon(GETrue);

        GEFloat fAnimationSpeed = 0.2f;
        gEAction aniAction = gEAction_Stumble;

        if (sCurrentMomventAni.Contains("_L."))
            aniAction = gEAction_StumbleL;
        else
            aniAction = gEAction_StumbleR;

        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), aniAction, "Hit", fAnimationSpeed);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    return GETrue;
}

ME_DEFINE_AND_REGISTER_SCRIPT_AI_FUNCTION(_AI_Parry)
{
    // Setup
    INIT_SCRIPT_STATE();
    gSArgsFor__AI_Parry param =
        *reinterpret_cast<gSArgsFor__AI_Parry *>(a_rRunTimeStack.GetAt(a_rRunTimeStack.GetCount() - 1).m_pArguments);

    // Execution in seperated Blocks!
    BREAK_BLOCK
    {
        ClearInputEntry(param.m_Self);

        param.m_Self.Routine.AccessProperty<PSRoutine::PropertyAction>() = gEAction_Parry; // Or add gEAction_Parry!
        param.m_Self.Routine.AccessProperty<PSRoutine::PropertyAniState>() = gEAniState_Stand;

        // Consume Staminapoints
        GetScriptAdmin().CallScriptFromScript("AddStaminaPoints", &param.m_Self, &None, -20);

        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Parry, "Raise", NBConfig::fParrySpeed);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {
        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Parry, "Hit", NBConfig::fParrySpeed);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {
        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_Parry, "Recover", NBConfig::fParrySpeed);

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }
    BREAK_BLOCK
    {}

    return GETrue;
}
