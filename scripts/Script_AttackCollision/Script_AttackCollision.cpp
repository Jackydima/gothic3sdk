#include "Script_AttackCollision.h"

gSScriptInit &GetScriptInit()
{
    static gSScriptInit s_ScriptInit;
    return s_ScriptInit;
}

GEBool CheckHandUseTypes(gEUseType p_lHand, gEUseType p_rHand, Entity &entity)
{
    return (entity.Inventory.GetItemFromSlot(gESlot_LeftHand).Interaction.GetUseType() == p_lHand
            && entity.Inventory.GetItemFromSlot(gESlot_RightHand).Interaction.GetUseType() == p_rHand);
}

void ClearInputEntry(Entity &a_Entity)
{
    using mFClearInputEntry = void(GE_STDCALL *)(Entity a_selfEntity);
    static mFClearInputEntry s_fClearInputEntry = force_cast<mFClearInputEntry>(RVA_ScriptGame(0x79a0));

    return s_fClearInputEntry(a_Entity);
}

GEFloat GE_STDCALL GetAnimationSpeedModifier(Entity &a_Entity, gEPhase a_Phase, gEAction a_Action)
{
    using mFGetAnimationSpeedModifier = GEFloat(GE_STDCALL *)(Entity, gEPhase);
    static mCCaller Call_GetAnimationSpeedModifier(mCCaller::GetCallerParams(RVA_ScriptGame(0x42a0), mERegisterType_Eax));

    Call_GetAnimationSpeedModifier.SetImmEax(a_Action);
    return Call_GetAnimationSpeedModifier.GetFunction<mFGetAnimationSpeedModifier>()(a_Entity, a_Phase);
}

void LoadSettings()
{
    eCConfigFile config = eCConfigFile();
    if (config.ReadFile(bCString("attacks.ini")))
    {
        ActiveAttackStartTime = config.GetFloat("Collision", "ActiveAttackStartTime", ActiveAttackStartTime);
        ActiveGetUpAttackStartTime =
            config.GetFloat("Collision", "ActiveGetUpAttackStartTime", ActiveGetUpAttackStartTime);
        ActivePierceAttackStartTime =
            config.GetFloat("Collision", "ActivePierceAttackStartTime", ActivePierceAttackStartTime);
        ActivePowerAttackStartTime =
            config.GetFloat("Collision", "ActivePowerAttackStartTime", ActivePowerAttackStartTime);
        ActivePowerAttack1H1HStartTime =
            config.GetFloat("Collision", "ActivePowerAttack1H1HStartTime", ActivePowerAttack1H1HStartTime);
        ActiveQuickAttackStartTime =
            config.GetFloat("Collision", "ActiveQuickAttackStartTime", ActiveQuickAttackStartTime);
        ActiveSimpleWhirlAttackStartTime =
            config.GetFloat("Collision", "ActiveSimpleWhirlAttackStartTime", ActiveSimpleWhirlAttackStartTime);
        ActiveWhirlAttackStartTime =
            config.GetFloat("Collision", "ActiveWhirlAttackStartTime", ActiveWhirlAttackStartTime);
        ActiveWhirlAttackSecondStartTime =
            config.GetFloat("Collision", "ActiveWhirlAttackSecondStartTime", ActiveWhirlAttackSecondStartTime);
    }
}

/*DECLARE_SCRIPT_FUNCTION(_AI_Attack)
{
    INIT_SCRIPT_STATE()
    BREAK_BLOCK
    {
        gEPose currentPose = SelfEntity.NPC.GetPrimaryPoseExt(gEAction_Attack, gEPhase_Hit);
        switch (currentPose)
        {
            case gEPose_P0:
            case gEPose_P2:
                SelfEntity.Routine.AccessProperty<PSRoutine::PropertyHitDirection>() = gEHitDirection_Right;
                break;
            case gEPose_P1:
            case gEPose_P3:
                SelfEntity.Routine.AccessProperty<PSRoutine::PropertyHitDirection>() = gEHitDirection_Left;
                break;
            default: break;
        }
    }
}*/

static mCFunctionHook Hook_OnAI_Attack;
DECLARE_SCRIPT_CALLBACK(OnAI_Attack)
{
    INIT_SCRIPT_CALLBACK()

    if (SelfEntity.IsPlayer())
    {
        Entity focusEntity = SelfEntity.Focus.GetFocusEntity(gECombatMode(2), gEDirection_None);
        if (focusEntity != None)
            SelfEntity.Focus.SetFocusEntity(focusEntity);
    }

    if (SelfEntity.Routine.GetProperty<PSRoutine::PropertyStatePosition>() == 0)
    {
        if (SelfEntity.Routine.GetStateTime() > ActiveAttackStartTime)
        {
            gEPose currentPose = SelfEntity.NPC.GetPrimaryPose();

            if (CheckHandUseTypes(gEUseType_1H, gEUseType_1H, SelfEntity) && currentPose == gEPose_P0)
            {
                Entity leftWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_LeftHand);
                leftWeapon.SetCollisionGroup(eECollisionGroup_Item_Attack);
                leftWeapon.TouchDamage.ClearTriggeredList();
            }
            else
            {
                // Any Pose, regular attacks are made by the right hand with Torch_1H
                Entity rightWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_RightHand);
                rightWeapon.SetCollisionGroup(eECollisionGroup_Item_Attack);
                rightWeapon.TouchDamage.ClearTriggeredList();
            }

            SelfEntity.Routine.AccessProperty<PSRoutine::PropertyStatePosition>() = 1;
        }
    }

    return GETrue;
}

static mCFunctionHook Hook_OnAI_GetUpAttack;
DECLARE_SCRIPT_CALLBACK(OnAI_GetUpAttack)
{
    INIT_SCRIPT_CALLBACK()

    if (SelfEntity.Routine.GetProperty<PSRoutine::PropertyStatePosition>() == 0)
    {
        if (SelfEntity.Routine.GetStateTime() > ActiveGetUpAttackStartTime)
        {
            SelfEntity.Routine.AccessProperty<PSRoutine::PropertyAniState>() = gEAniState_Stand;
            Entity rightWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_RightHand);
            rightWeapon.SetCollisionGroup(eECollisionGroup_Item_Attack);
            rightWeapon.TouchDamage.ClearTriggeredList();

            SelfEntity.Routine.AccessProperty<PSRoutine::PropertyStatePosition>() = 1;
        }
    }

    return GETrue;
}

static mCFunctionHook Hook_OnAI_PierceAttack;
DECLARE_SCRIPT_CALLBACK(OnAI_PierceAttack)
{
    INIT_SCRIPT_CALLBACK()

    if (SelfEntity.Routine.GetProperty<PSRoutine::PropertyStatePosition>() == 0)
    {
        if (SelfEntity.Routine.GetStateTime() > ActivePierceAttackStartTime)
        {
            gEPose currentPose = SelfEntity.NPC.GetPrimaryPose();
            if (CheckHandUseTypes(gEUseType_1H, gEUseType_1H, SelfEntity) && currentPose == gEPose_P1)
            {
                Entity leftWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_LeftHand);
                leftWeapon.SetCollisionGroup(eECollisionGroup_Item_Attack);
                leftWeapon.TouchDamage.ClearTriggeredList();
            }
            else
            {
                Entity rightWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_RightHand);
                rightWeapon.SetCollisionGroup(eECollisionGroup_Item_Attack);
                rightWeapon.TouchDamage.ClearTriggeredList();
            }

            SelfEntity.Routine.AccessProperty<PSRoutine::PropertyStatePosition>() = 1;
        }
    }

    return GETrue;
}

static mCFunctionHook Hook_OnAI_PowerAttack;
DECLARE_SCRIPT_CALLBACK(OnAI_PowerAttack)
{
    INIT_SCRIPT_CALLBACK()

    if (SelfEntity.IsPlayer())
    {
        Entity focusEntity = SelfEntity.Focus.GetFocusEntity(gECombatMode(2), gEDirection_Fwd);
        if (focusEntity != None)
            SelfEntity.Focus.SetFocusEntity(focusEntity);
    }

    if (SelfEntity.Routine.GetProperty<PSRoutine::PropertyStatePosition>() == 0)
    {
        if (SelfEntity.Routine.GetStateTime() > ActivePowerAttackStartTime)
        {
            Entity rightWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_RightHand);
            rightWeapon.SetCollisionGroup(eECollisionGroup_Item_Attack);
            rightWeapon.TouchDamage.ClearTriggeredList();

            if (CheckHandUseTypes(gEUseType_1H, gEUseType_1H, SelfEntity))
            {
                Entity leftWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_LeftHand);
                leftWeapon.SetCollisionGroup(eECollisionGroup_Item_Attack);
                leftWeapon.TouchDamage.ClearTriggeredList();
            }

            SelfEntity.Routine.AccessProperty<PSRoutine::PropertyStatePosition>() = 1;
        }
    }

    else if (SelfEntity.Routine.GetProperty<PSRoutine::PropertyStatePosition>() == 1)
    {
        if (SelfEntity.Routine.GetStateTime() > ActivePowerAttack1H1HStartTime)
        {
            if (CheckHandUseTypes(gEUseType_1H, gEUseType_1H, SelfEntity))
            {
                Entity rightWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_RightHand);
                rightWeapon.TouchDamage.ClearTriggeredList();

                Entity leftWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_LeftHand);
                leftWeapon.TouchDamage.ClearTriggeredList();
            }

            SelfEntity.Routine.AccessProperty<PSRoutine::PropertyStatePosition>() = 2;
        }
    }

    return GETrue;
}

static mCFunctionHook Hook_OnAI_QuickAttack;
DECLARE_SCRIPT_CALLBACK(OnAI_QuickAttack)
{
    INIT_SCRIPT_CALLBACK()

    if (SelfEntity.IsPlayer())
    {
        Entity focusEntity = SelfEntity.Focus.GetFocusEntity(gECombatMode(2), gEDirection_None);
        if (focusEntity != None)
            SelfEntity.Focus.SetFocusEntity(focusEntity);
    }

    if (SelfEntity.Routine.GetProperty<PSRoutine::PropertyStatePosition>() == 0)
    {
        if (SelfEntity.Routine.GetStateTime() > ActiveQuickAttackStartTime)
        {
            gEPose currentPose = SelfEntity.NPC.GetPrimaryPose();
            if ((CheckHandUseTypes(gEUseType_1H, gEUseType_1H, SelfEntity)
                 || CheckHandUseTypes(gEUseType_Torch, gEUseType_1H, SelfEntity))
                && currentPose == gEPose_P1)
            {
                Entity leftWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_LeftHand);
                leftWeapon.SetCollisionGroup(eECollisionGroup_Item_Attack);
                leftWeapon.TouchDamage.ClearTriggeredList();
            }
            else
            {
                Entity rightWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_RightHand);
                rightWeapon.SetCollisionGroup(eECollisionGroup_Item_Attack);
                rightWeapon.TouchDamage.ClearTriggeredList();
            }

            SelfEntity.Routine.AccessProperty<PSRoutine::PropertyStatePosition>() = 1;
        }
    }

    return GETrue;
}

static mCFunctionHook Hook_OnAI_SimpleWhirl;
DECLARE_SCRIPT_CALLBACK(OnAI_SimpleWhirl)
{
    INIT_SCRIPT_CALLBACK()

    if (SelfEntity.IsPlayer())
    {
        Entity focusEntity = SelfEntity.Focus.GetFocusEntity(gECombatMode(2), gEDirection_None);
        if (focusEntity != None)
            SelfEntity.Focus.SetFocusEntity(focusEntity);
    }

    if (SelfEntity.Routine.GetProperty<PSRoutine::PropertyStatePosition>() == 0)
    {
        if (SelfEntity.Routine.GetStateTime() > ActiveSimpleWhirlAttackStartTime)
        {
            gEPose currentPose = SelfEntity.NPC.GetPrimaryPose();
            if (CheckHandUseTypes(gEUseType_1H, gEUseType_1H, SelfEntity) && currentPose == gEPose_P1)
            {
                Entity leftWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_LeftHand);
                leftWeapon.SetCollisionGroup(eECollisionGroup_Item_Attack);
                leftWeapon.TouchDamage.ClearTriggeredList();
            }
            else
            {
                Entity rightWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_RightHand);
                rightWeapon.SetCollisionGroup(eECollisionGroup_Item_Attack);
                rightWeapon.TouchDamage.ClearTriggeredList();
            }

            SelfEntity.Routine.AccessProperty<PSRoutine::PropertyStatePosition>() = 1;
        }
    }

    return GETrue;
}

static mCFunctionHook Hook_OnAI_WhirlAttack;
DECLARE_SCRIPT_CALLBACK(OnAI_WhirlAttack)
{
    INIT_SCRIPT_CALLBACK()

    if (SelfEntity.Routine.GetProperty<PSRoutine::PropertyStatePosition>() == 0)
    {
        if (SelfEntity.Routine.GetStateTime() > ActiveWhirlAttackStartTime)
        {
            Entity rightWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_RightHand);
            rightWeapon.SetCollisionGroup(eECollisionGroup_Item_Attack);
            rightWeapon.TouchDamage.ClearTriggeredList();
            // rightWeapon.TouchDamage.AccessProperty<PSTouchDamage::PropertyResetOnUntouch>() = GETrue;
            SelfEntity.Routine.AccessProperty<PSRoutine::PropertyStatePosition>() = 1;
        }
    }

    if (SelfEntity.Routine.GetProperty<PSRoutine::PropertyStatePosition>() == 1)
    {
        if (SelfEntity.Routine.GetStateTime() > ActiveWhirlAttackSecondStartTime)
        {
            Entity rightWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_RightHand);
            rightWeapon.TouchDamage.ClearTriggeredList();
            SelfEntity.Routine.AccessProperty<PSRoutine::PropertyStatePosition>() = 2;
        }
    }

    return GETrue;
}

static mCFunctionHook Hook_AI_HackAttack;
DECLARE_SCRIPT_FUNCTION(_AI_HackAttack)
{
    INIT_SCRIPT_STATE();
    gSArgsFor__AI_HackAttack param = *reinterpret_cast<gSArgsFor__AI_HackAttack *>(
        a_rRunTimeStack.GetAt(a_rRunTimeStack.GetCount() - 1).m_pArguments);

    BREAK_BLOCK
    {
        ClearInputEntry(SelfEntity);

        param.m_Self.Routine.AccessProperty<PSRoutine::PropertyAction>() = gEAction_HackAttack;
        param.m_Self.Routine.AccessProperty<PSRoutine::PropertyAniState>() = gEAniState_Parade;

        GetScriptAdmin().CallScriptFromScript("AddStaminaPoints", &param.m_Self, &None, -20);

        Entity rightHandWeapon = param.m_Self.Inventory.GetItemFromSlot(gESlot_RightHand);
        rightHandWeapon.Strip.AccessProperty<PSStrip::PropertyEnabled>() = GETrue;

        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_HackAttack, "Raise",
            GetAnimationSpeedModifier(SelfEntity, gEPhase_Hit, gEAction_HackAttack));

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }

    BREAK_BLOCK
    {
        GetScriptAdmin().CallScriptFromScript("AddStaminaPoints", &param.m_Self, &None, -20);

        Entity rightHandWeapon = param.m_Self.Inventory.GetItemFromSlot(gESlot_RightHand);
        rightHandWeapon.SetCollisionGroup(eECollisionGroup_Item_Attack);
        rightHandWeapon.TouchDamage.ClearTriggeredList();

        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_HackAttack, "Hit",
            GetAnimationSpeedModifier(SelfEntity, gEPhase_Hit, gEAction_HackAttack));

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }

    BREAK_BLOCK
    { 
        Entity rightHandWeapon = SelfEntity.Inventory.GetItemFromSlot(gESlot_RightHand);
        rightHandWeapon.Strip.AccessProperty<PSStrip::PropertyEnabled>() = GEFalse;
        rightHandWeapon.SetCollisionGroup(eECollisionGroup_Item_Equipped);

        gCScriptProcessingUnit::sAICombatMoveInstr_Args combatMoveInstrArgs(
            param.m_Self.GetInstance(), param.m_Other.GetInstance(), gEAction_HackAttack, "Recover",
            GetAnimationSpeedModifier(SelfEntity, gEPhase_Hit, gEAction_HackAttack));

        if (!gCScriptProcessingUnit::sAICombatMoveInstr(&combatMoveInstrArgs, a_pSPU, GEFalse))
        {
            return GEFalse;
        }
    }

    BREAK_BLOCK
    {}

    return GETrue;
}

extern "C" __declspec(dllexport) gSScriptInit const *GE_STDCALL ScriptInit(void)
{
    GetScriptAdmin().LoadScriptDLL("Script_Game.dll");

    LoadSettings();

    Hook_OnAI_Attack.Hook(GetScriptAdminExt().GetScriptAICallback("OnAI_Attack")->m_funcScriptAICallback, &OnAI_Attack);
    Hook_OnAI_GetUpAttack.Hook(GetScriptAdminExt().GetScriptAICallback("OnAI_GetUpAttack")->m_funcScriptAICallback,
                               &OnAI_GetUpAttack);
    Hook_OnAI_PierceAttack.Hook(GetScriptAdminExt().GetScriptAICallback("OnAI_PierceAttack")->m_funcScriptAICallback,
                                &OnAI_PierceAttack);
    Hook_OnAI_QuickAttack.Hook(GetScriptAdminExt().GetScriptAICallback("OnAI_QuickAttack")->m_funcScriptAICallback,
                               &OnAI_QuickAttack);
    Hook_OnAI_PowerAttack.Hook(GetScriptAdminExt().GetScriptAICallback("OnAI_PowerAttack")->m_funcScriptAICallback,
                               &OnAI_PowerAttack);
    Hook_OnAI_SimpleWhirl.Hook(GetScriptAdminExt().GetScriptAICallback("OnAI_SimpleWhirl")->m_funcScriptAICallback,
                               &OnAI_SimpleWhirl);
    Hook_OnAI_WhirlAttack.Hook(GetScriptAdminExt().GetScriptAICallback("OnAI_WhirlAttack")->m_funcScriptAICallback,
                               &OnAI_WhirlAttack);

    Hook_AI_HackAttack.Hook(GetScriptAdminExt().GetScriptAIFunction("_AI_HackAttack")->m_funcScriptAIFunction,
                            &_AI_HackAttack);
    return &GetScriptInit();
}

//
// Entry Point
//

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH: ::DisableThreadLibraryCalls(hModule); break;
        case DLL_PROCESS_DETACH: break;
    }
    return TRUE;
}
