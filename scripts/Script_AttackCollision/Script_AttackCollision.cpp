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
        Entity focusEntity = SelfEntity.Focus.GetFocusEntity(gECombatMode(2), gEDirection_Fwd);
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

extern "C" __declspec(dllexport) gSScriptInit const *GE_STDCALL ScriptInit(void)
{
    GetScriptAdmin().LoadScriptDLL("Script_Game.dll");
    Hook_OnAI_Attack.Hook(GetScriptAdminExt().GetScriptAICallback("OnAI_Attack")->m_funcScriptAICallback, &OnAI_Attack);
    Hook_OnAI_GetUpAttack.Hook(GetScriptAdminExt().GetScriptAICallback("OnAI_GetUpAttack")->m_funcScriptAICallback, &OnAI_GetUpAttack);
    Hook_OnAI_PowerAttack.Hook(GetScriptAdminExt().GetScriptAICallback("OnAI_PowerAttack")->m_funcScriptAICallback, &OnAI_PowerAttack);
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
