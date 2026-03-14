#include "CallHook.h"

/**
 * Fix the hit detection of the PowerAttack of the Dual 1H PowerAttack
 */
static mCCallHook Hook_FixDualOneHanded;
// gCProcessinUnit at esp+0x2b8
void FixDualOneHanded(gCScriptProcessingUnit *p_SPU)
{
    if (p_SPU == NULL)
        return;

    Entity self = (Entity)p_SPU->GetSelfEntity();
    if (self == None)
        return;

    if (!CheckHandUseTypesNB(gEUseType_1H, gEUseType_1H, self))
    {
        return;
    }
    Entity rightHandWeapon = self.Inventory.GetItemFromSlot(gESlot_RightHand);
    rightHandWeapon.TouchDamage.ClearTriggeredList();

    Entity leftHandWeapon = self.Inventory.GetItemFromSlot(gESlot_LeftHand);
    leftHandWeapon.TouchDamage.ClearTriggeredList();
}

/**
 * Changed the Default Projectile Velocity for NPCs and give them Better Aiming
 */
static mCCallHook Hook_Shoot_Velocity;
void Shoot_Velocity(gCScriptProcessingUnit *p_PSU, Entity *p_self, Entity *p_target, PSProjectile *p_projectile)
{
    UNREFERENCED_PARAMETER(p_PSU);

    // Check for nullptr
    if (p_self == NULL || p_target == NULL)
        return;

    // Check if Self/Target Entity got removed during aiming
    if (*p_target == None || *p_self == None)
        return;

    // Only NPCs have "AutoAim"
    if (*p_self == Entity::GetPlayer())
        return;

    Entity projectileItem = p_self->Inventory.GetItemFromSlot(gESlot_RightHand);
    p_projectile->AccessProperty<PSProjectile::PropertyShootVelocity>() = static_cast<GEFloat>(NBConfig::shootVelocity);

    eCVisualAnimation_PS *targetAnimation = (eCVisualAnimation_PS *)(p_target->Animation.m_pEngineEntityPropertySet);
    if (targetAnimation == NULL)
        return;

    GEFloat time = projectileItem.GetDistanceTo(*p_target)
                 / static_cast<GEFloat>(NBConfig::shootVelocity * NBConfig::PROJECTILEMULTIPLICATOR);
    bCString actor = targetAnimation->GetActor()->GetActorName();
    actor.GetWord(1, "_", actor, GETrue, GETrue);
    if (actor.CompareFast("Boar"))
        actor = bCString("Keiler");
    if (actor.CompareFast("Alligator"))
        actor = bCString("Aligator");
    bCString spineName = actor + "_Spine_Spine_3";
    bCMatrix targetBoneMatrix;
    GEBool gotBoneMatrix = targetAnimation->GetBoneMatrix(spineName, targetBoneMatrix);
    if (!gotBoneMatrix)
    {
        spineName = actor + "_Spine_Spine_1";
        gotBoneMatrix = targetAnimation->GetBoneMatrix(spineName, targetBoneMatrix);
    }

    if (!gotBoneMatrix)
        return;

    bCVector targetVecPos = targetBoneMatrix.AccessTranslation();
    bCVector targetVec = targetVecPos - projectileItem.GetPosition();

    // Absolute Randomness of shots
    targetVecPos.AccessX() += (Entity::GetRandomNumber(static_cast<GEInt>(200 * NBConfig::NPC_AIM_INACCURACY))
                               - 100 * NBConfig::NPC_AIM_INACCURACY);
    // targetVec.AccessY ( ) += ( Entity::GetRandomNumber ( 50 ) - 25 );
    targetVecPos.AccessZ() += (Entity::GetRandomNumber(static_cast<GEInt>(200 * NBConfig::NPC_AIM_INACCURACY))
                               - 100 * NBConfig::NPC_AIM_INACCURACY);

    bCVector newTargetDirectionVec =
        (targetVecPos + (p_target->GetGameEntity()->GetLinearVelocity() * time)) - projectileItem.GetPosition();

    // Speed relative Randomness of shots
    GEFloat xDiff = newTargetDirectionVec.GetX() - targetVec.GetX();
    GEFloat zDiff = newTargetDirectionVec.GetZ() - targetVec.GetZ();
    newTargetDirectionVec.AccessX() +=
        (Entity::GetRandomNumber(static_cast<GEInt>(xDiff * NBConfig::NPC_AIM_INACCURACY * 2))
         - xDiff * NBConfig::NPC_AIM_INACCURACY);
    newTargetDirectionVec.AccessZ() +=
        (Entity::GetRandomNumber(static_cast<GEInt>(zDiff * NBConfig::NPC_AIM_INACCURACY * 2))
         - zDiff * NBConfig::NPC_AIM_INACCURACY);

    p_projectile->AccessProperty<PSProjectile::PropertyPathStyle>() = gEProjectilePath::gEProjectilePath_Line;
    p_projectile->SetTarget(None);
    p_projectile->AccessProperty<PSProjectile::PropertyTargetDirection>() = newTargetDirectionVec.GetNormalized();
}

static mCCallHook Hook_CombatMoveScale;
void CombatMoveScale(void *p_Ptr, gCScriptProcessingUnit *p_PSU, bCVector *vec)
{
    UNREFERENCED_PARAMETER(p_Ptr);
    Entity Self = (Entity)p_PSU->GetSelfEntity();
    // std::cout << "Name: " << Self.GetName ( ) << "\nRoutine: " << Self.Routine.GetProperty<PSRoutine::PropertyAction>
    // ( ) << "\n";
    switch (Self.Routine.GetProperty<PSRoutine::PropertyAction>())
    {
        case gEAction_Stumble:
        case gEAction_StumbleL:
        case gEAction_StumbleR:
        case gEAction_QuickParadeStumble:
        case gEAction_HeavyParadeStumble:
        case gEAction_QuickStumble:
        case gEAction_SitKnockDown:
        case gEAction_ParadeStumble:
        case gEAction_ParadeStumbleL:
        case gEAction_ParadeStumbleR:
        case gEAction_PierceStumble:      vec->Scale(0.5f); break;
        case gEAction_LieKnockDown:
        case gEAction_LieKnockOut:
        case gEAction_FinishingAttack:    break;
        case gEAction_HackAttack:
            if (Self.NPC.GetProperty<PSNpc::PropertySpecies>() == gESpecies_Orc)
                vec->Scale(2.17f * NBConfig::ATTACK_REACH_MULTIPLIER);
            else
                vec->Scale(1.2f * NBConfig::ATTACK_REACH_MULTIPLIER);
            break;
        case gEAction_QuickAttack:
        case gEAction_QuickAttackR:
        case gEAction_QuickAttackL: vec->Scale(0.85f * NBConfig::ATTACK_REACH_MULTIPLIER); break;
        case gEAction_PowerAttack:  vec->Scale(1.2f * NBConfig::ATTACK_REACH_MULTIPLIER); break;
        default:                    vec->Scale(NBConfig::ATTACK_REACH_MULTIPLIER);
    }
}

/**
 * Adjusted Bow Effect on Aiming for Player
 */
static mCCallHook Hook_PS_Ranged_PowerAim;
void PS_Ranged_PowerAim(void *p_Ptr, gCScriptProcessingUnit *p_PSU, void *esp)
{
    UNREFERENCED_PARAMETER(p_Ptr);
    Entity player = (Entity)p_PSU->GetSelfEntity();
    void *address = &NBConfig::powerAimEffectName;
    if (player.Inventory.IsSkillActive("Perk_Bow_3"))
    {
        address = &NBConfig::powerAimEliteEffectName;
    }
    else if (player.Inventory.IsSkillActive("Perk_Bow_2"))
    {
        address = &NBConfig::powerAimWarriorEffectName;
    }
    DWORD currProt, newProt;
    VirtualProtect((LPVOID)esp, sizeof(void *), PAGE_EXECUTE_READWRITE, &currProt);
    memset((LPVOID)esp, 0x90, sizeof(void *));
    memcpy((LPVOID)esp, address, sizeof(void *));
    VirtualProtect((LPVOID)esp, sizeof(void *), currProt, &newProt);
}

static mCCallHook Hook_ZS_Ranged_PowerAim;
void ZS_Ranged_PowerAim(void *p_Ptr, gCScriptProcessingUnit *p_PSU, void *esp)
{
    UNREFERENCED_PARAMETER(p_Ptr);
    Entity Self = (Entity)p_PSU->GetSelfEntity();
    void *address = &NBConfig::powerAimEffectName;

    if (getPowerLevel(Self) >= 40)
    {
        address = &NBConfig::powerAimEliteEffectName;
    }
    else if (getPowerLevel(Self) >= 30)
    {
        address = &NBConfig::powerAimWarriorEffectName;
    }
    DWORD currProt, newProt;
    VirtualProtect((LPVOID)esp, sizeof(void *), PAGE_EXECUTE_READWRITE, &currProt);
    memset((LPVOID)esp, 0x90, sizeof(void *));
    memcpy((LPVOID)esp, address, sizeof(void *));
    VirtualProtect((LPVOID)esp, sizeof(void *), currProt, &newProt);
}

static mCCallHook Hook_AssureProjectiles;
void AssureProjectiles(GEInt registerBaseStack)
{
    Entity *self = (Entity *)(registerBaseStack - 0x2A0);
    // std::cout << "Self: " << self->GetName() << "\n";
    if (*self == None)
    {
        // std::cout << "Unlucky" << "\n";
        return;
    }
    GEInt random = Entity::GetRandomNumber(10);
    GEInt leftHandWeaponIndex = self->Inventory.FindStackIndex(gESlot_LeftHand);
    Hook_AssureProjectiles.SetImmEbx<GEInt>(leftHandWeaponIndex);
    gEUseType leftHandUseType = self->Inventory.GetUseType(leftHandWeaponIndex);
    Template projectile = getProjectile(*self, leftHandUseType);
    // std::cout << "UseType: " << leftHandUseType << "\n";
    GEInt stack = self->Inventory.AssureItems(projectile, 0, random + 10);
    *(GEInt *)(registerBaseStack - 0x2C4) = stack;
}

static mCCallHook Hook_GiveXPPowerlevel;
void GiveXPPowerlevel(gCNPC_PS *p_npc)
{
    Entity entity = (Entity)p_npc->GetEntity();
    GEInt powerLevel = getPowerLevel(entity);
    Hook_GiveXPPowerlevel.SetImmEax(powerLevel);
}

static mCCallHook Hook_Evade;
void EvadeMechanic(gCScriptProcessingUnit *a_PSU)
{
    Entity Self = Entity(a_PSU->GetSelfEntity());
    Entity Other = Entity(a_PSU->GetOtherEntity());
    println("Call Hooked OnPlayerGamePressed function in jump session logic");
    eCKeyboard &keyboard = eCApplication::GetInstance().GetKeyboard();
    println("Address of Keyboard: %x", &keyboard);

    // Maps SessionKeys to physical Keyboard Keys
    // Protected constructor workaround
    gCSessionKeys sessionKeys = gCSessionKeys();
    sessionKeys = gCSession::GetInstance().GetSessionKeys();

    // Index 0 is default cursor arrows, Index 1 is the alternative keybinds (WASD)
    eCPhysicalKey *backKey = sessionKeys.GetAssignedKey(gESessionKey_Backward, 0);
    eCPhysicalKey *leftKey = sessionKeys.GetAssignedKey(gESessionKey_StrafeLeft, 0);
    eCPhysicalKey *rightKey = sessionKeys.GetAssignedKey(gESessionKey_StrafeRight, 0);

    eCPhysicalKey *backKeyAlt = sessionKeys.GetAssignedKey(gESessionKey_Backward, 1);
    eCPhysicalKey *leftKeyAlt = sessionKeys.GetAssignedKey(gESessionKey_StrafeLeft, 1);
    eCPhysicalKey *rightKeyAlt = sessionKeys.GetAssignedKey(gESessionKey_StrafeRight, 1);

    GEBool backPressed = keyboard.KeyPressed(backKey->m_enuKeyboardStateOffset)
                      || keyboard.KeyPressed(backKeyAlt->m_enuKeyboardStateOffset);
    GEBool leftPressed = keyboard.KeyPressed(leftKey->m_enuKeyboardStateOffset)
                      || keyboard.KeyPressed(leftKeyAlt->m_enuKeyboardStateOffset);
    GEBool rightPressed = keyboard.KeyPressed(rightKey->m_enuKeyboardStateOffset)
                       || keyboard.KeyPressed(rightKeyAlt->m_enuKeyboardStateOffset);

    Hook_Evade.SetImmEax(1);

    // Usually it would be better to add an action instead of hard setting states,
    // for the current task loop to handle, but we want less hooks :)

    // Do to allow spamming Evading or Canceling forced recovery in other states!
    //gEAction selfCurrentAction = Self.Routine.GetProperty<PSRoutine::PropertyAction>();
    //if (selfCurrentAction == gEAction_Evade || selfCurrentAction == gEAction_PierceAttack
    //    || selfCurrentAction == gEAction_HackAttack || selfCurrentAction == gEAction_Parry)
    if (!Self.Routine.GetCurrentState().Contains("_Loop"))
    {
        return;
    }

    // if (keyboard.KeyPressed(eCInpShared::eEKeyboardStateOffset_A))
    if (leftPressed && !rightPressed)
    {
        println("Left Evade Initialized");
        Self.Routine.SetState("NB_EvadeLeft");
        return;
    }

    if (rightPressed && !leftPressed)
    {
        println("Right Evade Initialized");
        Self.Routine.SetState("NB_EvadeRight");
        return;
    }

    if (backPressed)
    {
        println("Back Evade Initialized");
        Self.Routine.SetState("NB_EvadeBackward");
        return;
    }

    GEInt retVal = GetScriptAdmin().CallScriptFromScript("OnPlayerJump", &Self, &None);
    Hook_Evade.SetImmEax(retVal);
}

void EvadeAniString(gCScriptProcessingUnit::sAICombatMoveInstr_Args *a_pArgs, gCScriptProcessingUnit *a_pSPU)
{
    if (a_pArgs->Action != gEAction_Evade)
        return;

    eCEntity *Self = a_pSPU->m_Speaker.GetEntity();
    if (!Self)
        return;

    bCString aniName;
    if (a_pArgs->PhaseName.Contains("Right"))
    {
        if (a_pArgs->PhaseName == "Right_Raise")
        {
            aniName = "Hero_EvadeRight_Raise";
        }

        else if (a_pArgs->PhaseName == "Right_Recover")
        {
            aniName = "Hero_EvadeRight_Recover";
        }

        else
        {
            aniName = "Hero_EvadeRight_Hit";
        }

        // p_SPU->m_DirectionVec = Self->GetWorldMatrix().GetXAxis().GetNormalized();
    }
    else if (a_pArgs->PhaseName.Contains("Left"))
    {
        if (a_pArgs->PhaseName == "Left_Raise")
        {
            aniName = "Hero_EvadeLeft_Raise";
        }

        else if (a_pArgs->PhaseName == "Left_Recover")
        {
            aniName = "Hero_EvadeLeft_Recover";
        }

        else
        {
            aniName = "Hero_EvadeLeft_Hit";
        }

        // p_SPU->m_DirectionVec = -Self->GetWorldMatrix().GetXAxis().GetNormalized();
    }
    else
    {
        if (a_pArgs->PhaseName == "Raise")
        {
            aniName = "Hero_EvadeBack_Raise";
        }

        else if (a_pArgs->PhaseName == "Recover")
        {
            aniName = "Hero_EvadeBack_Recover";
        }

        else
        {
            aniName = "Hero_EvadeBack_Hit";
        }

        // p_SPU->m_DirectionVec = -Self->GetWorldMatrix().GetZAxis().GetNormalized();
    }
    a_pSPU->m_strAniString.SetText(aniName);
}

void ParryAniString(gCScriptProcessingUnit::sAICombatMoveInstr_Args *a_pArgs, gCScriptProcessingUnit *a_pSPU)
{
    if (a_pArgs->Action != gEAction_Parry)
    {
        return;
    }

    eCEntity *Self = a_pSPU->m_Speaker.GetEntity();
    if (!Self)
    {
        return;
    }

    /*gCNPC_PS *selfNPC = GetPropertySet<gCNPC_PS>(Self, eEPropertySetType_NPC);
    if (!selfNPC)
    {
        a_pSPU->m_strAniString.SetText("");
        return;
    }


    gESpecies selfSpecies = selfNPC->GetSpecies();
    if (selfSpecies != gESpecies_Human && selfSpecies != gESpecies_Orc)
        return;

    eCVisualAnimation_PS *selfAnimation = GetPropertySet<eCVisualAnimation_PS>(Self, eEPropertySetType_Animation);
    if (!selfAnimation)
        return;

    auto actor = selfAnimation->GetActor();

    if (!actor)
        return;

    bCString aniName = actor->GetActorName();

    aniName += "_Stand_";*/

    // For now leave the naming convention, but force P0 pose!
    bCString strPose = "";
    a_pSPU->m_strAniString.GetWord(4, "_", strPose, GETrue, GETrue);

    a_pSPU->m_strAniString.Replace(strPose, "P0");
}

static mCCallHook Hook_CombatMoveStartAniString;
void CombatMoveStartAniString(gCScriptProcessingUnit::sAICombatMoveInstr_Args *a_pArgs, gCScriptProcessingUnit *a_pSPU)
{
    EvadeAniString(a_pArgs, a_pSPU);
    ParryAniString(a_pArgs, a_pSPU);
}

void EvadeMovement(gCScriptProcessingUnit::sAICombatMoveInstr_Args *a_pArgs, gCScriptProcessingUnit *a_pSPU)
{
    if (a_pArgs->Action != gEAction_Evade)
        return;

    eCEntity *Self = a_pSPU->m_Speaker.GetEntity();
    if (!Self)
        return;

    if (a_pSPU->m_SelfMovementPS)
    {
        a_pSPU->m_SelfMovementPS->EnableCombatRotationFromSPU(GEFalse, 0.0f);
    }

    if (a_pSPU->m_SelfNavigationPS)
    {
        a_pSPU->m_SelfNavigationPS->SetCurrentAniDirection(gEDirection_Fwd);
    }

    if (!a_pSPU->m_strAniString.Contains("Hit"))
        return;

    const GEFloat evadeDistance = 250.0f;

    if (a_pArgs->PhaseName.Contains("Right"))
    {
        a_pSPU->m_DirectionVec = Self->GetWorldMatrix().GetXAxis().GetNormalized();
    }
    else if (a_pArgs->PhaseName.Contains("Left"))
    {
        a_pSPU->m_DirectionVec = -Self->GetWorldMatrix().GetXAxis().GetNormalized();
    }
    else
    {
        a_pSPU->m_DirectionVec = -Self->GetWorldMatrix().GetZAxis().GetNormalized();
    }

    eCWrapper_emfx2Actor *selfActor = a_pSPU->m_SelfAnimationPS->GetActor();
    GEFloat maxTime = static_cast<GEFloat>(selfActor->GetMaxTime(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst));
    GEFloat aniLength = maxTime / a_pArgs->AniSpeedScale;
    a_pSPU->m_DirectionVec *= evadeDistance / aniLength;
}

void ParryMovement(gCScriptProcessingUnit::sAICombatMoveInstr_Args *a_pArgs, gCScriptProcessingUnit *a_pSPU)
{
    if (a_pArgs->Action != gEAction_Parry)
        return;

    eCEntity *Self = a_pSPU->m_Speaker.GetEntity();
    if (!Self)
        return;

    if (a_pSPU->m_SelfMovementPS)
    {
        // a_pSPU->m_SelfMovementPS->EnableCombatRotationFromSPU(GEFalse, 0.0f);
    }

    if (a_pSPU->m_SelfNavigationPS)
    {
        a_pSPU->m_SelfNavigationPS->SetCurrentAniDirection(gEDirection_Fwd);
    }

    if (!a_pSPU->m_strAniString.Contains("Hit"))
        return;

    a_pSPU->m_DirectionVec = bCVector(0.0f, 0.0f, 0.0f);
}

static mCCallHook Hook_CombatMoveStartMovement;
void CombatMoveStartMovement(gCScriptProcessingUnit::sAICombatMoveInstr_Args *a_pArgs, gCScriptProcessingUnit *a_pSPU)
{
    EvadeMovement(a_pArgs, a_pSPU);
    ParryMovement(a_pArgs, a_pSPU);
}

void HookCallHooks()
{
    // Add Parry String to g_pstrActionString!
    // Start of ActionArray string! 0x3f75c0
    // is normally const!! but we need to add things there...
    bCString *p_sParry = reinterpret_cast<bCString *>(RVA_Game(0x3f75c0 + 0x4 * static_cast<GEInt>(gEAction_Parry)));
    *p_sParry = bCString("Parry");

    Hook_CombatMoveStartMovement.Prepare(RVA_Game(0x16b8a9), &CombatMoveStartMovement)
        .InsertCall()
        .AddPtrStackArgEbp(0x8)
        .AddPtrStackArgEbp(0xC)
        .RestoreRegister()
        .Hook();

    Hook_CombatMoveStartAniString.Prepare(RVA_Game(0x16b065), &CombatMoveStartAniString)
        .InsertCall()
        .AddPtrStackArgEbp(0x8)
        .AddPtrStackArgEbp(0xC)
        .RestoreRegister()
        .Hook();

    Hook_Evade.Prepare(RVA_ScriptGame(0x62119), &EvadeMechanic, mCBaseHook::mEHookType_Mixed, mERegisterType_Eax)
        .InsertCall()
        .AddPtrStackArgEbp(0x8)
        .RestoreRegister()
        .ReplaceSize(0x6212f - 0x62119)
        .Hook();

    Hook_FixDualOneHanded.Prepare(RVA_ScriptGame(0x482e7), &FixDualOneHanded, mCBaseHook::mEHookType_OnlyStack)
        .InsertCall()
        .AddPtrStackArg(0x2B8)
        .RestoreRegister()
        .Hook();

    Hook_CombatMoveScale
        .Prepare(RVA_Game(0x16b8a3), &CombatMoveScale, mCBaseHook::mEHookType_Mixed, mERegisterType::mERegisterType_Ecx)
        .InsertCall()
        .AddPtrStackArgEbp(0x8)
        .AddPtrStackArgEbp(0xC)
        .AddRegArg(mERegisterType::mERegisterType_Ecx)
        .RestoreRegister()
        .Hook();

    Hook_AssureProjectiles
        .Prepare(RVA_ScriptGame(0x192a2), &AssureProjectiles, mCBaseHook::mEHookType_Mixed,
                 mERegisterType::mERegisterType_Ebx)
        .InsertCall()
        .AddRegArg(mERegisterType::mERegisterType_Ebp)
        .ReplaceSize(0x1932b - 0x192a2)
        .RestoreRegister()
        .Hook();

    if (NBConfig::adjustXPReceive)
    {
        Hook_GiveXPPowerlevel
            .Prepare(RVA_ScriptGame(0x4e451), &GiveXPPowerlevel, mCBaseHook::mEHookType_Mixed,
                     mERegisterType::mERegisterType_Eax)
            .InsertCall()
            .AddPtrStackArg(0x11c)
            .ReplaceSize(0x4e45a - 0x4e451)
            .RestoreRegister()
            .Hook();
    }

    if (NBConfig::useNewBowMechanics)
    {
        Hook_Shoot_Velocity.Prepare(RVA_ScriptGame(0x86882), &Shoot_Velocity)
            .InsertCall()
            .AddPtrStackArgEbp(0x8)
            .AddPtrStackArgEbp(0xC)
            .AddPtrStackArgEbp(0x10)
            .AddStackArg(0xB8)
            .RestoreRegister()
            .Hook();

        Hook_PS_Ranged_PowerAim.Prepare(RVA_ScriptGame(0x84940), &PS_Ranged_PowerAim)
            .InsertCall()
            .AddPtrStackArgEbp(0x8)
            .AddPtrStackArgEbp(0xC)
            .AddStackArg(0)
            .RestoreRegister()
            .Hook();

        Hook_ZS_Ranged_PowerAim.Prepare(RVA_ScriptGame(0x195f6), &ZS_Ranged_PowerAim)
            .InsertCall()
            .AddPtrStackArgEbp(0x8)
            .AddPtrStackArgEbp(0xC)
            .AddStackArg(0)
            .RestoreRegister()
            .Hook();
    }
}
