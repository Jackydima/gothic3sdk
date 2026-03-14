#include "FunctionHook.h"

mCFunctionHook Hook_GetAnimationSpeedModifier;
GEFloat GE_STDCALL GetAnimationSpeedModifier(Entity a_Entity, gEPhase a_Phase)
{
    gEAction a_Action = Hook_GetAnimationSpeedModifier.GetImmEax<gEAction>(); // param 1

    gESpecies species = a_Entity.NPC.GetProperty<PSNpc::PropertySpecies>();
    // gEAction a_Action = a_Entity.Routine.GetProperty<PSRoutine::PropertyAction>();
    GEInt staminaPoints = GetScriptAdmin().CallScriptFromScript("GetStaminaPoints", &a_Entity, &None, 0);
    GEBool isArenaNPC = a_Entity != Entity::GetPlayer()
                     && a_Entity.NPC.GetProperty<PSNpc::PropertyAttackReason>() == gEAttackReason_Arena;
    GEFloat multiPlier = 1.0f;
    GEBool isHumanInFistMode = GetScriptAdmin().CallScriptFromScript("IsHumanoid", &a_Entity, &None)
                            && GetScriptAdmin().CallScriptFromScript("IsInFistMode", &a_Entity, &None);

    Entity Player = Entity::GetPlayer();

    if (staminaPoints <= 20)
    {
        if (isArenaNPC)
            multiPlier = 0.9f;
        else
            multiPlier = 0.8f;
    }
    else if (staminaPoints <= 50)
    {
        if (isArenaNPC)
            multiPlier = 0.95f;
        else
            multiPlier = 0.9f;
    }

    if (a_Action == gEAction_SprintAttack)
        return 1.5f;
    if (species == gESpecies_Troll)
    {
        if (a_Action == gEAction_PowerAttack)
            return 1.5f;
        return 1.3f;
    }

    if (a_Entity.NPC.IsDiseased())
        multiPlier *= 0.9f;

    if (isArenaNPC)
        multiPlier *= NBConfig::npcArenaSpeedMultiplier; // default 1.25

    // Alternative MonsterRage Mode, Monsters are just faster now!
    if (NBConfig::MonsterRageModus != 0 && NBConfig::MonsterRageModus != 1)
    {
        if (a_Entity.NPC.GetCurrentTarget() == Player && CanRage(a_Entity)
            && Player.NPC.GetProperty<PSNpc::PropertyLastHitTimestamp>() >= 120)
        {
            multiPlier *= 1.5;
            // Every Rage decreases the RageMode a bit!
            GEInt newTimeStamp = Player.NPC.GetProperty<PSNpc::PropertyLastHitTimestamp>() - 10;
            Player.NPC.AccessProperty<PSNpc::PropertyLastHitTimestamp>() = Max(0, newTimeStamp);
        }
    }

    switch (a_Action)
    {
        case gEAction_FinishingAttack:
        case gEAction_SitKnockDown:
        case gEAction_LieKnockDown:
        case gEAction_LieKnockOut:
        case gEAction_LieDead:
        case gEAction_LiePiercedKO:
        case gEAction_LiePiercedDead:  return 1.0f;
        case gEAction_Aim:
        case gEAction_Reload:
            if (a_Entity == Player && NBConfig::useNewBowMechanics)
            {
                if (a_Entity.Inventory.IsSkillActive("Perk_Bow_3"))
                {
                    return NBConfig::animationSpeedBonusHigh;
                }
                if (a_Entity.Inventory.IsSkillActive("Perk_Bow_2"))
                {
                    return NBConfig::animationSpeedBonusMid;
                }
            }
            else if (NBConfig::useNewBowMechanics)
            {
                Entity weapon = a_Entity.GetWeapon(GETrue);
                if (weapon.Interaction.GetUseType() == gEUseType_CrossBow)
                {
                    if (getPowerLevel(a_Entity) >= NBConfig::uniqueLevel)
                        return 2.5f;
                    if (getPowerLevel(a_Entity) >= NBConfig::warriorLevel)
                        return 1.75f;
                }
                if (getPowerLevel(a_Entity) >= NBConfig::uniqueLevel)
                    return NBConfig::animationSpeedBonusHigh;
                if (getPowerLevel(a_Entity) >= NBConfig::warriorLevel)
                    return NBConfig::animationSpeedBonusMid;
            }
            return 1.0f;
        case gEAction_Cast:
        case gEAction_PowerCast:
            if (a_Entity.GetName().Contains("Xardas"))
                return 1.3f;
            return 1.0f;
        case gEAction_Attack:
            if (isHumanInFistMode)
                return 0.54f;
            if (CheckHandUseTypesNB(gEUseType_None, gEUseType_1H, a_Entity))
                return 0.6f * multiPlier;
            if (CheckHandUseTypesNB(gEUseType_Shield, gEUseType_1H, a_Entity))
                return 0.6f * multiPlier;
            if (CheckHandUseTypesNB(gEUseType_Torch, gEUseType_1H, a_Entity))
                return 0.6f * multiPlier;
            if (CheckHandUseTypesNB(gEUseType_1H, gEUseType_1H, a_Entity))
                return 0.6f * multiPlier;
            if (CheckHandUseTypesNB(gEUseType_None, gEUseType_2H, a_Entity))
                return 0.7f * multiPlier;
            if (CheckHandUseTypesNB(gEUseType_None, gEUseType_Axe, a_Entity))
                return 0.7f * multiPlier;
            if (CheckHandUseTypesNB(gEUseType_None, gEUseType_Staff, a_Entity))
                return 0.7f * multiPlier;
            if (CheckHandUseTypesNB(gEUseType_None, gEUseType_Halberd, a_Entity))
                return 0.7f * multiPlier;
            return 1 * multiPlier;
        case gEAction_PowerAttack:
            if (isHumanInFistMode)
                return 0.6f;
            if (species == gESpecies_Orc && a_Phase == gEPhase_Raise)
                return 1.3f * multiPlier; // orcs
            if (a_Phase == gEPhase_Raise)
                return 1.5f * multiPlier;
            if ((CheckHandUseTypesNB(gEUseType_1H, gEUseType_1H, a_Entity)))
                return 0.9f * multiPlier;
        case gEAction_QuickAttackR:
        case gEAction_QuickAttackL:
        case gEAction_PierceAttack:
        case gEAction_HackAttack:
        case gEAction_PierceStumble:  return 1 * multiPlier;
        case gEAction_WhirlAttack:    return 0.9f;
        case gEAction_SimpleWhirl:    return 1.3f * multiPlier;
        case gEAction_Parade:
        case gEAction_ParadeR:
        case gEAction_ParadeL:        return 2.5f * multiPlier; // default 2.3
        case gEAction_ParadeStumble:
        case gEAction_ParadeStumbleR:
        case gEAction_ParadeStumbleL: return 2.0f * multiPlier;
        case gEAction_HeavyParadeStumble:
            if (CheckHandUseTypesNB(gEUseType_1H, gEUseType_1H, a_Entity))
                return 0.7f * multiPlier;
            return 1 * multiPlier;
        case gEAction_QuickStumble: return 2 * multiPlier;
        case gEAction_Stumble:
        case gEAction_StumbleR:
        case gEAction_StumbleL:     return 1.3f * multiPlier;
        case gEAction_Summon:       return 1.5f * multiPlier;
        case gEAction_FlameSword:   return 0.7f * multiPlier;

        case gEAction_Cock:
            if (a_Entity == Entity::GetPlayer())
            {
                if (a_Entity.Inventory.IsSkillActive("Perk_Crossbow_3"))
                    return 2.5f;
                if (a_Entity.Inventory.IsSkillActive("Perk_Crossbow_2"))
                    return 1.75f;
                return 1.0f;
            }

            if (getPowerLevel(a_Entity) >= NBConfig::uniqueLevel)
            {
                return 2.5f;
            }
            if (getPowerLevel(a_Entity) >= NBConfig::warriorLevel)
            {
                return 1.75f;
            }
            return 1;

        default: return 1 * multiPlier;
    }
}

GEInt OnPowerAim_Loop(gCScriptProcessingUnit *p_PSU)
{
    Entity Self = (Entity)p_PSU->GetSelfEntity();
    Entity Other = (Entity)p_PSU->GetOtherEntity();

    Entity focusEntity = Self.Focus.GetFocusEntity((gECombatMode)3, gEDirection_None);
    Self.Focus.SetFocusEntity(focusEntity);

    Entity item = Self.Inventory.GetItemFromSlot(gESlot_RightHand);

    GEFloat stateTime = Self.Routine.GetStateTime();

    /**
     * Here the regular Bow Animation has 50 Frames: The Statetime for 100% is 1.95 Seconds
     */
    GEFloat hitMultiplier = stateTime / 1.95f;
    if (Self == Entity::GetPlayer())
    {
        if (Self.Inventory.IsSkillActive("Perk_Bow_3"))
            hitMultiplier *= NBConfig::animationSpeedBonusHigh;
        else if (Self.Inventory.IsSkillActive("Perk_Bow_2"))
            hitMultiplier *= NBConfig::animationSpeedBonusMid;
    }
    else
    {
        if (getPowerLevel(Self) >= 40)
            hitMultiplier *= NBConfig::animationSpeedBonusHigh;
        else if (getPowerLevel(Self) >= 30)
            hitMultiplier *= NBConfig::animationSpeedBonusMid;
    }
    if (hitMultiplier < 0.75)
        hitMultiplier /= 2;
    if (hitMultiplier > 1)
        hitMultiplier = 1;

    item.Damage.AccessProperty<PSDamage::PropertyDamageHitMultiplier>() = hitMultiplier;

    GEInt *flagForPuttingWeaponBack = (GEInt *)(RVA_ScriptGame(0x118d58));

    if (*flagForPuttingWeaponBack)
    {
        gEAction action = (gEAction) * *(gEAction **)(RVA_ScriptGame(0x118d54));
        typedef void (*Func1)(Entity);
        typedef void (*Func2)(GEInt, BYTE, Entity);
        Func1 func1 = (Func1)RVA_ScriptGame(0x79a0);
        Func2 func2 = (Func2)RVA_ScriptGame(0x2e50);
        if (action == gEAction_AbortAttack)
        {
            func1(Self);
            func2(-1, BYTE(-1), Self);
            Self.Routine.FullStop();
            Self.Routine.SetTask("PS_Ranged_Reload");
            Self.Routine.SetState("PS_Ranged_Reload");
            return 1;
        }

        if (action == gEAction_Shoot)
        {
            func1(Self);
            Self.Routine.FullStop();
            Self.Routine.SetState("PS_Ranged_Shoot");
        }
    }
    return 1;
}

static std::map<bCString, GEInt> LastHealthDamageMap = {};
static std::map<bCString, GEInt> LastStaminaUsageMap = {};
static GEInt healthRecoveryDelay = 60;

GEInt HealthUpdateOnTickHelper(Entity &p_entity, GEInt p_healthValue)
{
    // std::cout << "HealthInc: " << p_healthValue << "\t Last Time: " << getLastHealthDamageTime (
    // p_entity.GetGameEntity ( )->GetID ( ).GetText ( ) ) << "\n";
    if (p_healthValue > 0
        && getLastTimeFromMap(p_entity.GetGameEntity()->GetID().GetText(), LastHealthDamageMap) < healthRecoveryDelay)
        return 0;
    return p_healthValue;
}

void ResetHitPointsRegen(Entity &p_entity)
{
    LastHealthDamageMap[p_entity.GetGameEntity()->GetID().GetText()] =
        Entity::GetWorldEntity().Clock.GetTimeStampInSeconds();
}

GEInt GE_STDCALL MagicPoison(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                             GEU32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);

    Entity spell = Self.Interaction.GetSpell();

    if (CanBePoisoned(a_pSPU, &Other, &spell, a_iArgs))
    {
        auto damageReceiver = static_cast<gCDamageReceiver_PS_Ext *>(
            Other.GetGameEntity()->GetPropertySet(eEPropertySetType_DamageReceiver));
        damageReceiver->AccessPoisonDamage() = GetPoisonDamage(Self);
        Other.NPC.EnableStatusEffects(gEStatusEffect_Poisoned, GETrue);
    }

    GetScriptAdmin().CallScriptFromScript("AssessTarget", &Other, &Self, gEAttackReason_ReactToDamage);

    return 1;
}

static mCFunctionHook Hook_AddHitPoints;
GEInt GE_STDCALL AddHitPoints(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                              GEI32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);
    if (a_iArgs < 0)
    {
        ResetHitPointsRegen(Self);
    }
    return Hook_AddHitPoints.GetOriginalFunction(&AddHitPoints)(a_pSPU, a_pSelfEntity, a_pOtherEntity, a_iArgs);
}

GEInt UpdateHitPointsOnTick(Entity p_entity)
{
    GEInt retVal = 0;
    gEAIMode aiMode = p_entity.Routine.GetProperty<PSRoutine::PropertyAIMode>();
    // GEBool aBActive = eCApplication::GetInstance ( ).GetEngineSetup ( ).AlternativeBalancing;
    if (aiMode == gEAIMode_Dead)
    {
        return 0;
    }

    if (p_entity.NPC.IsBurning())
        retVal -= 5;

    if (p_entity.NPC.IsPoisoned())
    {
        gCDamageReceiver_PS_Ext *damageReceiver = static_cast<gCDamageReceiver_PS_Ext *>(
            p_entity.GetGameEntity()->GetPropertySet(eEPropertySetType_DamageReceiver));
        GEU32 poisonDamage = damageReceiver->GetPoisonDamage();
        if (poisonDamage <= 0)
            poisonDamage = 5;
        retVal -= poisonDamage;
    }
    if (p_entity.NPC.IsFrozen())
    {
        retVal -= 2;
    }

    if (p_entity.NPC.IsInMagicBarrier())
        retVal -= 10;

    if (retVal < 0)
        return HealthUpdateOnTickHelper(p_entity, retVal);

    if (p_entity.IsPlayer() && p_entity.Inventory.IsSkillActive("Perk_MasterGladiator"))
    {
        GEInt maxHealth = p_entity.PlayerMemory.GetHitPointsMax();
        retVal += static_cast<GEInt>(maxHealth * 0.01);
    }
    else if (!p_entity.IsPlayer()
             && p_entity.Party.AccessProperty<PSParty::PropertyPartyMemberType>() != gEPartyMemberType_Summoned
             && getPowerLevel(p_entity) >= NBConfig::bossLevel)
    {
        GEInt maxHealth = p_entity.DamageReceiver.GetProperty<PSDamageReceiver::PropertyHitPointsMax>();
        retVal += static_cast<GEInt>(maxHealth * 0.01);
    }

    if (retVal > 0)
    {
        if (aiMode == gEAIMode_Combat)
            retVal = static_cast<GEInt>(retVal * 0.5);
        if (retVal < 3)
        {
            retVal = 3;
        }
    }

    gEAniState aniState = p_entity.Routine.GetProperty<PSRoutine::PropertyAniState>();
    switch (aniState)
    {
        case gEAniState_SitThrone:
        case gEAniState_SitGround:
        case gEAniState_SitStool:
        case gEAniState_SitBench:
        case gEAniState_SleepGround:
        case gEAniState_SleepBed:     retVal += 5; break;
        case gEAniState_SitKnockDown:
        case gEAniState_LieKnockDown: return 0;
    }
    return HealthUpdateOnTickHelper(p_entity, retVal);
}

GEInt GE_STDCALL CanParade(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity, GEU32 a_iArgs)
{
    INIT_SCRIPT_EXT(Victim, DamagerOwner);
    UNREFERENCED_PARAMETER(a_iArgs);

    // Only Parade With Melee weapons!
    if (GetHeldWeaponCategoryNB(Victim) != gEWeaponCategory_Melee)
        return GEFalse;

    if (Victim.NPC.HasStatusEffects(gEStatusEffect_Frozen))
        return GEFalse;

    // Only Block when in FOV!
    if (!Victim.IsInFOV(DamagerOwner))
        return GEFalse;

    gCScriptAdmin &ScriptAdmin = GetScriptAdmin();
    gEAction victimAction = Victim.Routine.GetProperty<PSRoutine::PropertyAction>();
    gEAniState victimAniState = Victim.Routine.GetProperty<PSRoutine::PropertyAniState>();

    // Special Block for melee weapons while beeing knockdowned!
    if (victimAniState == gEAniState_SitKnockDown || victimAction == gEAction_GetUpParade)
    {
        if (GetHeldWeaponCategoryNB(DamagerOwner) == gEWeaponCategory_Melee)
        {
            return GETrue;
        }
    }

    if (!ScriptAdmin.CallScriptFromScript("IsInParadeMode", &Victim, &None, 0))
        return GEFalse;

    if (!ScriptAdmin.CallScriptFromScript("CanParadeMoveOf", &Victim, &DamagerOwner, 0))
        return GEFalse;

    if (victimAction == gEAction_HackAttack)
        return GEFalse;

    if (eCApplication::GetInstance().GetEngineSetup().AlternativeAI
        && (victimAction == gEAction_PierceAttack || victimAction == gEAction_WhirlAttack
            || victimAction == gEAction_FinishingAttack))
    {
        return GEFalse;
    }
    return GETrue;
}

DECLARE_SCRIPT(CanParadeMoveOf)
{
    INIT_SCRIPT_EXT(Self, Other);
    UNREFERENCED_PARAMETER(a_iArgs);

    GEBool bVictimHasShield = CheckHandUseTypesNB(gEUseType_Shield, gEUseType_1H, Self);
    gESpecies otherSpecies = Other.NPC.GetProperty<PSNpc::PropertySpecies>();
    gCScriptAdmin &ScriptAdmin = GetScriptAdmin();

    if (ScriptAdmin.CallScriptFromScript("IsInFistMode", &Other, &None))
    {
        // Human Fists can be blocked always!
        if (ScriptAdmin.CallScriptFromScript("IsHumanoid", &Other, &None))
        {
            return GETrue;
        }

        switch (otherSpecies)
        {
            case gESpecies_Minecrawler:
            case gESpecies_Sabertooth:
            case gESpecies_Shadowbeast:
            case gESpecies_Bison:
            case gESpecies_Rhino:
            case gESpecies_Ripper:
            case gESpecies_Alligator:
            case gESpecies_Golem:
            case gESpecies_FireGolem:
            case gESpecies_IceGolem:
            case gESpecies_ScorpionKing: return bVictimHasShield;

            case gESpecies_Troll:
            case gESpecies_Trex:
            case gESpecies_Gargoyle:
                // No Shield returns False
                if (!bVictimHasShield)
                    return GEFalse;

                if (Self.IsPlayer())
                    return Self.Inventory.IsSkillActive("Perk_Shield_2");

                return (35 <= Self.NPC.GetProperty<PSNpc::PropertyLevelMax>());
        }

        // Humans can not block monster fists with fists!
        if (ScriptAdmin.CallScriptFromScript("IsInFistMode", &Self, &None))
        {
            return GEFalse;
        }

        // Small Monsters can be block with weapons now
        return GETrue;
    }

    // Fist cannot block weapons!
    if (ScriptAdmin.CallScriptFromScript("IsInFistMode", &Self, &None))
    {
        return GEFalse;
    }

    // Strong Monsters with weapons!
    if (otherSpecies == gESpecies_Demon || otherSpecies == gESpecies_Ogre)
    {
        if (!Self.IsPlayer())
        {
            return 35 <= Self.NPC.GetProperty<PSNpc::PropertyLevelMax>();
        }

        if (bVictimHasShield && Self.Inventory.IsSkillActive("Perk_Shield_2"))
            return GETrue;

        if (CheckHandUseTypesNB(gEUseType_None, gEUseType_1H, Self) && Self.Inventory.IsSkillActive("Perk_1H_3"))
            return GETrue;

        if (CheckHandUseTypesNB(gEUseType_1H, gEUseType_1H, Self) && Self.Inventory.IsSkillActive("Perk_1H1H_2"))
            return GETrue;

        if ((CheckHandUseTypesNB(gEUseType_None, gEUseType_Axe, Self)
             || CheckHandUseTypesNB(gEUseType_None, gEUseType_2H, Self)
             || CheckHandUseTypesNB(gEUseType_None, gEUseType_Halberd, Self)
             || CheckHandUseTypesNB(gEUseType_None, gEUseType_Pickaxe, Self))
            && Self.Inventory.IsSkillActive("Perk_Axe_3"))
        {
            return GETrue;
        }

        if (CheckHandUseTypesNB(gEUseType_None, gEUseType_Staff, Self) && Self.Inventory.IsSkillActive("Perk_Staff_3"))
            return GETrue;

        return GEFalse;
    }

    switch (Other.Routine.GetProperty<PSRoutine::PropertyAction>())
    {
        case gEAction_PowerAttack:
            // Dual Wielded 1H can get through blocks against victims only at the initial startup of the attack
            if (CheckHandUseTypesNB(gEUseType_1H, gEUseType_1H, Other)
                && 1 >= Other.Routine.GetProperty<PSRoutine::PropertyStatePosition>())
            {
                // Can not block initial Attack of dual wielding powerattack!
                return GEFalse;
            }
            break;
        case gEAction_PierceAttack:
            if (NBConfig::useExtendedBlocking)
            {
                if (CheckHandUseTypesNB(gEUseType_1H, gEUseType_1H, Self))
                    return GETrue;
            }
            if (bVictimHasShield)
                return GETrue;

            return GEFalse;
        case gEAction_HackAttack:
            if (NBConfig::useExtendedBlocking)
            {
                if (IsHoldingTwoHandedWeapon(Self))
                    return GETrue;
            }
            if (bVictimHasShield)
                return GETrue;

            return GEFalse;
        default: break;
    }

    // default
    return GETrue;
}

static mCFunctionHook Hook_OnTick;
GEInt GE_STDCALL OnTick(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity, GEI32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);

    if (Self.IsPlayer() && IsPlayerInCombat())
    {
        Self.NPC.AccessProperty<PSNpc::PropertyCombatState>() = 1;
    }
    else if (Self.IsPlayer())
    {
        Self.NPC.AccessProperty<PSNpc::PropertyCombatState>() = 0;
    }

    // std::cout << "Entity: "<< p_entity.GetName() << " --- CombatState: " <<
    // p_entity.NPC.GetProperty<PSNpc::PropertyCombatState> ( ) << "\n";

    // Innos lights now actually tries to banish the darkness of beliar :)
    gEAIMode selfAIMode = Self.Routine.GetProperty<PSRoutine::PropertyAIMode>();
    gESpecies selfSpecies = Self.NPC.GetProperty<PSNpc::PropertySpecies>();
    if (NBConfig::useDamagingInnosLight && selfAIMode != gEAIMode_Down && selfAIMode != gEAIMode_Dead
        && selfSpecies != gESpecies_Golem && selfSpecies != gESpecies_IceGolem && selfSpecies != gESpecies_FireGolem
        && !Self.NPC.IsBurning() && GetScriptAdmin().CallScriptFromScript("IsEvil", &Self, &None))
    {
        auto entityList = Self.GetEntitiesByDistance();
        Entity currentEntity;
        for (GEInt i = 0; i < entityList.GetCount(); i++)
        {
            currentEntity = entityList.GetAt(i);
            // print ( "Entity: %s is at distance: %f\n" , currentEntity.GetName ( ) , Self.GetDistanceTo (
            // currentEntity ) );
            if (Self.GetDistanceTo(currentEntity) > 500)
                break;

            if (currentEntity.GetName() == "Smn_Light")
            {
                Self.NPC.EnableStatusEffects(gEStatusEffect_Burning, GETrue);
                Self.Effect.StartRuntimeEffect("eff_magic_firespell_target_01");
                break;
            }
        }
    }

    return Hook_OnTick.GetOriginalFunction(&OnTick)(a_pSPU, a_pSelfEntity, a_pOtherEntity, a_iArgs);
}

static mCFunctionHook Hook_MagicTransform;
GEInt GE_STDCALL MagicTransform(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                                GEI32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);

    // TODO: Check for NPCs Transformation
    if (!Self.IsPlayer())
        return 1;

    Entity transformedSpawn = Self.Interaction.GetSpell().Magic.GetSpawn();
    Template spawn = transformedSpawn.GetTemplate();
    bCMatrix pos = Self.GetPose();

    Entity spawnedEntity = Entity::Spawn(spawn, pos);
    if (!spawnedEntity.FindSpawnPose(pos, spawnedEntity, GETrue, 0))
    {
        EffectSystem::StartEffect("CAST_FAIL", pos, spawnedEntity);
        spawnedEntity.Kill();
        return 0;
    }

    spawnedEntity.MoveTo(pos);

    GEInt transformDuration =
        GetScriptAdmin().CallScriptFromScript("GetTransformDuration", a_pSelfEntity, a_pOtherEntity, 0);
    if (Self.Inventory.GetCategory(a_iArgs) == gEWeaponCategory::gEWeaponCategory_Magic)
    {
        transformDuration = -1;
    }
    GetScriptAdmin().CallScriptFromScript("SendPerceptionTransform", &Self, &None, 0);

    Self.StartTransform(spawnedEntity, static_cast<GEFloat>(transformDuration), GEFalse);
    GEInt leftHand = speciesLeftHand(spawnedEntity);
    GEInt rightHand = speciesRightHand(spawnedEntity);

    spawnedEntity.Inventory.HoldStacks(leftHand, rightHand);

    // TODO extra Functions
    typedef void (*Func1)(GEInt, GEInt, Entity);
    typedef void (*Func2)(Entity);
    Func1 func1 = (Func1)RVA_ScriptGame(0x2e50);
    Func2 func2 = (Func2)RVA_ScriptGame(0x7a00);
    func1(leftHand, rightHand, spawnedEntity);
    func2(spawnedEntity);

    spawnedEntity.Routine.SetTask("PS_Melee");

    return 1;
}

// TODO: Check if Other stats should affect the scaling
static mCFunctionHook Hook_StartTransform;
void GE_STDCALL StartTransform(Entity *p_targetEntity, GEFloat p_duration, GEBool p_bool)
{
    Entity Self = *Hook_StartTransform.GetSelf<Entity *>();
    // std::cout << "Start: " << Self.GetName() << "\n";
    // TODO Check for NPCs
    if (!Self.IsPlayer())
        return;
    if (!p_bool)
    {
        Self.GetGameEntity()->Enable(GEFalse);
        Self.EnableCollision(GEFalse);
        gCEntity *entity = Self.GetGameEntity();
        gCDynamicCollisionCircle_PS *dcc =
            (gCDynamicCollisionCircle_PS *)entity->GetPropertySet(eEPropertySetType_DynamicCollisionCircle);
        if (dcc != 0)
        {
            Self.Navigation.SetDCCEnabled(GEFalse);
        }
    }

    if (Self.PlayerMemory.IsValid())
    {
        Self.PlayerMemory.AccessProperty<PSPlayerMemory::PropertySecondsTransformRemain>() = p_duration;
    }

    GEInt playerLevel = Self.NPC.GetProperty<PSNpc::PropertyLevel>();
    GEBool isDruid = Self.Inventory.IsSkillActive("Perk_Druid");
    GEBool isWaterMage = Self.Inventory.IsSkillActive("Perk_Watermage");
    GEBool hasManaRegen = Self.Inventory.IsSkillActive("Perk_MasterMage");
    GEInt targetLevel = p_targetEntity->NPC.GetProperty<PSNpc::PropertyLevelMax>();

    /**
     * �fter TakeOver, The Player Takes over the targetEntity
     * The TargetEntity had no PlayerMemory and after the TakeOver, it gets the PlayerMem
     * with every Attribute of the Player on 100
     * Also the Self Entity (PC_Hero) is no longer the Player in checks
     * The Player can be taken from the Entity::GetPlayer() function, which also
     * has most of the Entities properties.
     * PlayerMemory and CharacterControll got added to the new Player Entity
     * Changes to the new Entity will not affect the PC_Hero Entity (default Player)
     */
    p_targetEntity->StartTakeOver();

    // needs to be this. Cant be targetEntity (or Self)
    Entity player = Entity::GetPlayer();

    if (player.Routine.IsValid())
        player.Routine.ContinueRoutine();

    player.NPC.EnableStatusEffects(gEStatusEffect::gEStatusEffect_Transformed, GETrue);

    if (isDruid)
        targetLevel = static_cast<GEInt>(targetLevel * 1.5f);

    if (isWaterMage)
        targetLevel += 10;

    GEInt newLevel = (targetLevel + playerLevel) / 2;

    p_targetEntity->NPC.AccessProperty<PSNpc::PropertyLevel>() = newLevel;
    p_targetEntity->NPC.AccessProperty<PSNpc::PropertyLevelMax>() = targetLevel;

    // Stats must be in PlayerMem for some reason ...
    // Either do it here, or adjust all the Other things
    GEInt healthStat = static_cast<GEInt>(p_targetEntity->NPC.GetProperty<PSNpc::PropertyLevelMax>() * 20);
    if (healthStat < 200)
    {
        healthStat = 200;
    }

    GEInt otherAttributStat = static_cast<GEInt>(p_targetEntity->NPC.GetProperty<PSNpc::PropertyLevelMax>() * 10);
    if (otherAttributStat < 100)
    {
        otherAttributStat = 100;
    }

    player.PlayerMemory.SetHitPointsMax(healthStat);
    player.PlayerMemory.SetHitPoints(healthStat);
    player.PlayerMemory.SetManaPointsMax(otherAttributStat);
    player.PlayerMemory.SetManaPoints(otherAttributStat);
    player.PlayerMemory.SetStaminaPointsMax(otherAttributStat);
    player.PlayerMemory.SetStaminaPoints(otherAttributStat);

    if (hasManaRegen)
    {
        p_targetEntity->Inventory.AssureItemsEx("It_Perk_MasterMage", 0, 1, -1, GETrue);
    }
}

static mCFunctionHook Hook_MagicSleep;
GEInt GE_STDCALL MagicSleep(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                            GEI32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);

    gCInventory_PS *inventory = (gCInventory_PS *)Other.Inventory.m_pEngineEntityPropertySet;

    inventory->UnlinkStackFromSlot(gESlot_RightHand);
    inventory->UnlinkStackFromSlot(gESlot_LeftHand);

    return Hook_MagicSleep.GetOriginalFunction(&MagicSleep)(a_pSPU, a_pSelfEntity, a_pOtherEntity, a_iArgs);
}

GEInt GE_STDCALL GetProtection(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                               GEI32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);
    GEInt protection = GetProtectionHUD(a_pSPU, a_pSelfEntity, a_pOtherEntity, a_iArgs);
    return static_cast<GEInt>(protection * NBConfig::playerArmorMultiplier);
}

GEInt GE_STDCALL GetProtectionHUD(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                                  GEI32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);
    if (Self == None)
        return 0;
    GEInt protection = 0;
    bCString protectionCheckString = "";

    gEDamageType damageType = Other.Damage.GetProperty<PSDamage::PropertyDamageType>();
    if (Other == None)
    {
        switch (a_iArgs)
        {
            case 13: damageType = gEDamageType_Lightning; break;
            case 14: damageType = gEDamageType_Ice; break;
            case 15: damageType = gEDamageType_Fire; break;
            case 16: damageType = gEDamageType_Missile; break;
            case 17: damageType = gEDamageType_Impact; break;
            default: damageType = gEDamageType_Blade;
        }
    }

    if (!Self.IsPlayer() || Self.NPC.IsTransformed())
    {
        protection = GetScriptAdmin().CallScriptFromScript("GetLevelMax", a_pSelfEntity, &None);
        protection = static_cast<GEInt>(protection * NBConfig::npcArmorMultiplier);
        GEInt stackIndexLeftHand = Self.Inventory.FindStackIndex(gESlot::gESlot_LeftHand);
        GEInt stackIndexLeftHandBack = Self.Inventory.FindStackIndex(gESlot::gESlot_BackLeft);
        if (Self.Inventory.GetUseType(stackIndexLeftHand) == gEUseType_Shield
            || Self.Inventory.GetUseType(stackIndexLeftHandBack) == gEUseType_Shield)
        {
            protection = static_cast<GEInt>(protection * 1.25f);
        }
        protection = static_cast<GEInt>(protection / NBConfig::playerArmorMultiplier);
        return protection;
    }
    else
    {
        // Maybe Use EAB here
        eCApplication::GetInstance().GetEngineSetup().AlternativeBalancing;
        switch (damageType)
        {
            case gEDamageType_Blade:
                protection = Self.PlayerMemory.GetProtectionBlades();
                protectionCheckString = "PROT_BLADE";
                break;
            case gEDamageType_Impact:
                protection = Self.PlayerMemory.GetProtectionImpact();
                protectionCheckString = "PROT_IMPACT";
                break;
            case gEDamageType_Missile:
                protection = Self.PlayerMemory.GetProtectionMissile();
                protectionCheckString = "PROT_MISSILE";
                break;
            case gEDamageType_Fire:
                protection = Self.PlayerMemory.GetProtectionFire();
                if (Self.Inventory.IsSkillActive("Perk_ResistHeat"))
                {
                    protection += NBConfig::elementalPerkBonusResistance;
                }
                protectionCheckString = "PROT_FIRE";
                break;
            case gEDamageType_Ice:
                protection = Self.PlayerMemory.GetProtectionIce();
                if (Self.Inventory.IsSkillActive("Perk_ResistCold"))
                {
                    protection += NBConfig::elementalPerkBonusResistance;
                }
                protectionCheckString = "PROT_ICE";
                break;
            case gEDamageType_Lightning:
                protection = Self.PlayerMemory.GetProtectionLightning();
                protectionCheckString = "PROT_LIGHTNING";
        }
        GEInt stackIndexBody = Self.Inventory.FindStackIndex(gESlot_Body);
        Entity bodyEntity = Self.Inventory.GetTemplateItem(stackIndexBody);
        if (bodyEntity == None)
        {
            return protection;
        }
        gCItem_PS *item = (gCItem_PS *)bodyEntity.Item.m_pEngineEntityPropertySet;
        GEInt itemProt = 0;
        if (item->GetModAttrib1Tag() == protectionCheckString)
        {
            itemProt = item->GetModAttrib1Value();
        }
        else if (item->GetModAttrib2Tag() == protectionCheckString)
        {
            itemProt = item->GetModAttrib2Value();
        }
        else if (item->GetModAttrib3Tag() == protectionCheckString)
        {
            itemProt = item->GetModAttrib3Value();
        }
        else if (item->GetModAttrib4Tag() == protectionCheckString)
        {
            itemProt = item->GetModAttrib4Value();
        }
        else if (item->GetModAttrib5Tag() == protectionCheckString)
        {
            itemProt = item->GetModAttrib5Value();
        }
        else if (item->GetModAttrib6Tag() == protectionCheckString)
        {
            itemProt = item->GetModAttrib6Value();
        }
        // Add the Robe Protection twice to the player if Skill is active
        if (bodyEntity.Item.IsRobe() && Self.Inventory.IsSkillActive("Perk_LightArmor"))
        {
            // elemental damage is more effective for robes
            if (damageType == gEDamageType_Fire || damageType == gEDamageType_Ice
                || damageType == gEDamageType_Lightning)
            {
                protection = static_cast<GEInt>(protection + (itemProt * 1.5f));
            }
            else
            {
                protection += itemProt;
            }
        }
        // Add 50% Extra Protection for ONLY the Body Armor now, maybe add helmet aswell
        else if (Self.Inventory.IsSkillActive("Perk_HeavyArmor"))
        {
            protection = static_cast<GEInt>(protection + (itemProt * 0.5f));
        }
        return protection;
    }
}

// TODO: Not FINISHED
/*
GEInt GE_STDCALL GetProtectionHUDAbsolute ( gCScriptProcessingUnit* a_pSPU , Entity* a_pSelfEntity , Entity*
a_pOtherEntity , GEI32 a_iArgs ) { INIT_SCRIPT_EXT ( Self , Other ); if ( Self == None ) return 0; GEInt protection = 0;
    bCString protectionCheckString = "";

    gEDamageType damageType = Other.Damage.GetProperty<PSDamage::PropertyDamageType> ( );
    if ( Other == None ) {
        switch ( a_iArgs ) {
        case 13:
            damageType = gEDamageType_Lightning;
            break;
        case 14:
            damageType = gEDamageType_Ice;
            break;
        case 15:
            damageType = gEDamageType_Fire;
            break;
        case 16:
            damageType = gEDamageType_Missile;
            break;
        case 17:
            damageType = gEDamageType_Impact;
            break;
        default:
            damageType = gEDamageType_Blade;
        }
    }

    if ( !Self.IsPlayer ( ) || Self.NPC.IsTransformed ( ) ) {
        protection = GetScriptAdmin ( ).CallScriptFromScript ( "GetLevelMax" , a_pSelfEntity , &None );
        protection *= npcArmorMultiplier;
        GEInt stackIndexLeftHand = Self.Inventory.FindStackIndex ( gESlot::gESlot_LeftHand );
        GEInt stackIndexLeftHandBack = Self.Inventory.FindStackIndex ( gESlot::gESlot_BackLeft );
        if ( Self.Inventory.GetUseType ( stackIndexLeftHand ) == gEUseType_Shield || Self.Inventory.GetUseType (
stackIndexLeftHandBack ) == gEUseType_Shield ) { protection *= 1.25;
        }
        protection /= playerArmorMultiplier;
    }

    GEBool playerNT = Self.IsPlayer ( ) && !Self.NPC.IsTransformed ( );
    // Maybe Use EAB here
    eCApplication::GetInstance ( ).GetEngineSetup ( ).AlternativeBalancing;
    switch ( damageType ) {
    case gEDamageType_Blade:
        if ( playerNT )
            protection = Self.PlayerMemory.GetProtectionBlades ( );
        protectionCheckString = "PROT_BLADE";
        break;
    case gEDamageType_Impact:
        if ( playerNT )
            protection = Self.PlayerMemory.GetProtectionImpact ( );
        protectionCheckString = "PROT_IMPACT";
        break;
    case gEDamageType_Missile:
        if ( playerNT )
            protection = Self.PlayerMemory.GetProtectionMissile ( );
        protectionCheckString = "PROT_MISSILE";
        break;
    case gEDamageType_Fire:
        if ( playerNT )
            protection = Self.PlayerMemory.GetProtectionFire ( );
        if ( Self.Inventory.IsSkillActive ( "Perk_ResistHeat" ) ) {
            protection += elementalPerkBonusResistance;
        }
        protectionCheckString = "PROT_FIRE";
        break;
    case gEDamageType_Ice:
        if ( playerNT )
            protection = Self.PlayerMemory.GetProtectionIce ( );
        if ( Self.Inventory.IsSkillActive ( "Perk_ResistCold" ) ) {
            protection += elementalPerkBonusResistance;
        }
        protectionCheckString = "PROT_ICE";
        break;
    case gEDamageType_Lightning:
        if ( playerNT )
            protection = Self.PlayerMemory.GetProtectionLightning ( );
        protectionCheckString = "PROT_LIGHTNING";
    }
    gCItem_PS* item;
    if ( playerNT ) {
        GEInt stackIndexBody = Self.Inventory.FindStackIndex ( gESlot_Body );
        Entity bodyEntity = Self.Inventory.GetTemplateItem ( stackIndexBody );
        if ( bodyEntity == None ) {
            return protection;
        }
        item = ( gCItem_PS* )bodyEntity.Item.m_pEngineEntityPropertySet;
    }
    else {
        Entity itemTemp = Self.Inventory.GetDefaultItemFromSlot ( gESlot_Body );
        item = ( gCItem_PS* )itemTemp.Item.m_pEngineEntityPropertySet;
    }
    GEInt itemProt = 0;
    if ( item->GetModAttrib1Tag ( ) == protectionCheckString ) {
        itemProt = item->GetModAttrib1Value ( );
    }
    else if ( item->GetModAttrib2Tag ( ) == protectionCheckString ) {
        itemProt = item->GetModAttrib2Value ( );
    }
    else if ( item->GetModAttrib3Tag ( ) == protectionCheckString ) {
        itemProt = item->GetModAttrib3Value ( );
    }
    else if ( item->GetModAttrib4Tag ( ) == protectionCheckString ) {
        itemProt = item->GetModAttrib4Value ( );
    }
    else if ( item->GetModAttrib5Tag ( ) == protectionCheckString ) {
        itemProt = item->GetModAttrib5Value ( );
    }
    else if ( item->GetModAttrib6Tag ( ) == protectionCheckString ) {
        itemProt = item->GetModAttrib6Value ( );
        //Add the Robe Protection twice to the player if Skill is active
        if ( bodyEntity.Item.IsRobe ( ) && Self.Inventory.IsSkillActive ( "Perk_LightArmor" ) ) {
            //elemental damage is more effective for robes
            if ( damageType == gEDamageType_Fire || damageType == gEDamageType_Ice || damageType ==
gEDamageType_Lightning ) { protection += ( itemProt * 1.5 );
            }
            else {
                protection += itemProt;
            }
        }
        // Add 50% Extra Protection for ONLY the Body Armor now, maybe add helmet aswell
        else if ( Self.Inventory.IsSkillActive ( "Perk_HeavyArmor" ) ) {
            protection += ( itemProt * 0.5 );
        }
        return protection;
    }
}*/

static mCFunctionHook Hook_GetCurrentLevel;
GEInt GE_STDCALL GetCurrentLevel(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                                 GEI32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);
    if (Self.IsPlayer())
    {
        return Hook_GetCurrentLevel.GetOriginalFunction(&GetCurrentLevel)(a_pSPU, a_pSelfEntity, a_pOtherEntity,
                                                                          a_iArgs);
    }
    return GetScriptAdmin().CallScriptFromScript("GetLevelMax", a_pSelfEntity, a_pOtherEntity, a_iArgs);
}
// TODO:
static mCFunctionHook Hook_GetMaxLevel;
GEInt GE_STDCALL GetMaxLevel(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                             GEI32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);
    if (Self.IsPlayer())
    {
        return Hook_GetMaxLevel.GetOriginalFunction(&GetMaxLevel)(a_pSPU, a_pSelfEntity, a_pOtherEntity, a_iArgs);
    }
    GEInt level = (Hook_GetMaxLevel.GetOriginalFunction(&GetMaxLevel)(a_pSPU, a_pSelfEntity, a_pOtherEntity, a_iArgs)
                   + Self.NPC.GetProperty<PSNpc::PropertyLevel>())
                / 2;
    return level;
}

GEInt StaminaUpdateOnTickHelper(Entity &p_entity, GEInt p_staminaValue)
{
    if (p_staminaValue > 0
        && getLastTimeFromMap(p_entity.GetGameEntity()->GetID().GetText(), LastStaminaUsageMap)
               < NBConfig::staminaRecoveryDelay)
        return 0;
    return p_staminaValue;
}

static mCFunctionHook Hook_StaminaUpdateOnTick;
GEInt StaminaUpdateOnTick(Entity p_entity)
{
    const GEInt standardStaminaRecovery =
        NBConfig::staminaRecoveryPerTick
        + GetScriptAdmin().CallScriptFromScript("GetStaminaPointsMax", &p_entity, &None, 0) / 100;
    // TODO Change that again;
    // GEInt retStaminaDelta = 0;

    if (p_entity.IsPlayer() && p_entity.Routine.GetProperty<PSRoutine::PropertyAction>() == gEAction::gEAction_Aim)
    {
        if (GetScriptAdmin().CallScriptFromScript("GetStaminaPoints", &p_entity, &None, 0) <= 7)
        {
            p_entity.Routine.FullStop();
            p_entity.Routine.SetState("PS_Normal");
            bCString aniname = p_entity.GetAni(gEAction_AbortAttack, gEPhase::gEPhase_Begin);
            p_entity.StartPlayAni(aniname, 0, GETrue, 0, GEFalse);
        }
        return StaminaUpdateOnTickHelper(p_entity, -7);
    }

    // For Now Only for player!
    if (p_entity.IsPlayer() && (p_entity.IsSprinting() || (p_entity.IsSwimming() && *(BYTE *)RVA_Executable(0x27FD2))))
    {
        if (p_entity.NPC.GetProperty<PSNpc::PropertySpecies>() == gESpecies_Bloodfly)
        {
            return StaminaUpdateOnTickHelper(p_entity, -1);
        }

        if (eCApplication::GetInstance().GetEngineSetup().AlternativeBalancing)
        {
            if (p_entity.Inventory.IsSkillActive(Template("Perk_Sprinter"))
                || (p_entity != Entity::GetPlayer() && getPowerLevel(p_entity) >= NBConfig::warriorLevel))
                return StaminaUpdateOnTickHelper(p_entity, -4);
            return StaminaUpdateOnTickHelper(p_entity, -8);
        }
        if (p_entity.Inventory.IsSkillActive(Template("Perk_Sprinter"))
            || (p_entity != Entity::GetPlayer() && getPowerLevel(p_entity) >= NBConfig::warriorLevel))
            return StaminaUpdateOnTickHelper(p_entity, -5);
        return StaminaUpdateOnTickHelper(p_entity, -10);
    }

    if (p_entity.IsJumping())
        return StaminaUpdateOnTickHelper(p_entity, 0);

    if (p_entity.NPC.IsDiseased())
        return StaminaUpdateOnTickHelper(p_entity, 1);

    // HoldingBlockFlag 0x118ab0
    if (*(BYTE *)RVA_ScriptGame(0x118ab0) && eCApplication::GetInstance().GetEngineSetup().AlternativeBalancing)
        return StaminaUpdateOnTickHelper(p_entity, 1);
    typedef GEU32(GetWeatherAdmin)(void);
    // Get eCWeatherAdmin *! also available at RVA_ScriptGame(0x11a210)
    GetWeatherAdmin *getWeatherAdminFunction = (GetWeatherAdmin *)RVA_ScriptGame(0x12e0);

    GEU32 weatherAdmin = getWeatherAdminFunction();
    // Or Temperatur
    GEFloat weatherCondition = *(GEFloat *)(weatherAdmin + 0xd0);

    // Maybe Add more complex logic for Npcs aswell bro
    if (weatherCondition >= 40.0)
    {
        if (p_entity.IsPlayer() && !p_entity.Inventory.IsSkillActive(Template("Perk_ResistHeat")))
            return StaminaUpdateOnTickHelper(p_entity, 2);
        return StaminaUpdateOnTickHelper(p_entity, standardStaminaRecovery);
    }

    if (weatherCondition <= -40.0)
    {
        if (p_entity.IsPlayer() && !p_entity.Inventory.IsSkillActive(Template("Perk_ResistCold")))
            return StaminaUpdateOnTickHelper(p_entity, 2);
        return StaminaUpdateOnTickHelper(p_entity, standardStaminaRecovery);
    }

    return StaminaUpdateOnTickHelper(p_entity, standardStaminaRecovery);
}

static mCFunctionHook Hook_AddStaminaPoints;
GEInt AddStaminaPoints(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity, GEI32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);
    if (a_iArgs < 0)
    {
        LastStaminaUsageMap[Self.GetGameEntity()->GetID().GetText()] =
            Entity::GetWorldEntity().Clock.GetTimeStampInSeconds();
    }
    return Hook_AddStaminaPoints.GetOriginalFunction(&AddStaminaPoints)(a_pSPU, a_pSelfEntity, a_pOtherEntity, a_iArgs);
}

static mCFunctionHook Hook_GetAttituteSummons;
GEInt GetAttitudeSummons(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity, GEU32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);
    gCScriptAdmin &ScriptAdmin = GetScriptAdmin();

    if (Self.Party.GetPartyLeader() != None && Self.Party.GetPartyLeader() == Other.Party.GetPartyLeader())
        return 1;

    if (Self.Party.GetProperty<PSParty::PropertyPartyMemberType>() == gEPartyMemberType_Summoned
        && Self.Party.GetPartyLeader() != Other && Self.Party.GetPartyLeader() != None)
    {
        if (!(Self.Party.GetPartyLeader().NPC.GetCurrentTarget() == Other
              && Self.Party.GetPartyLeader().NPC.GetProperty<PSNpc::PropertyCombatState>() == 1))
        {
            Entity selfLeader = Self.Party.GetPartyLeader();
            GEInt retVal = ScriptAdmin.CallScriptFromScript("GetAttitude", &selfLeader, &Other, a_iArgs);
            if (retVal != 4)
                return 2;
            return retVal;
        }
    }

    if (Other.Party.GetProperty<PSParty::PropertyPartyMemberType>() == gEPartyMemberType_Summoned
        && Other.Party.GetPartyLeader() != Self && Other.Party.GetPartyLeader() != None)
    {
        if (!(Other.Party.GetPartyLeader().NPC.GetCurrentTarget() == Self
              && Other.Party.GetPartyLeader().NPC.GetProperty<PSNpc::PropertyCombatState>() == 1))
        {
            Entity otherLeader = Other.Party.GetPartyLeader();
            GEInt retVal = ScriptAdmin.CallScriptFromScript("GetAttitude", &otherLeader, &Self, a_iArgs);
            if (retVal != 4)
                return 2;
            return retVal;
        }
    }
    return Hook_GetAttituteSummons.GetOriginalFunction(&GetAttitudeSummons)(a_pSPU, a_pSelfEntity, a_pOtherEntity,
                                                                            a_iArgs);
}

/**
 * Addition to the CanFreeze for new Spell!
 * Always return true on IceBlock spell
 */
static mCFunctionHook Hook_CanFreeze;
GEInt CanFreezeAddition(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity, GEU32 a_iArgs)
{
    // Fix for new Spell
    INIT_SCRIPT_EXT(Victim, Damager);
    if (Damager != None && Damager.GetName() == "Mis_IceBlock"
        && Victim.NPC.GetProperty<PSNpc::PropertySpecies>() != gESpecies_IceGolem)
        return GETrue;
    return Hook_CanFreeze.GetOriginalFunction(&CanFreezeAddition)(a_pSPU, a_pSelfEntity, a_pOtherEntity, a_iArgs);
}

GEInt GetQualityBonus(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity, GEU32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);

    GEInt retValue = 0;
    GEU32 itemQuality = Self.Item.GetQuality();

    if ((itemQuality & gEItemQuality_Worn) == gEItemQuality_Worn)
    {
        retValue = static_cast<GEInt>(retValue - (a_iArgs * ((GEFloat)(100 - NBConfig::wornPercentageMalus) / 100.0f)));
    }

    if ((itemQuality & gEItemQuality_Sharp) == gEItemQuality_Sharp)
    {
        if (NBConfig::useSharpPercentage)
        {
            GEU32 l_sharpBonus = static_cast<GEU32>(a_iArgs * ((GEFloat)NBConfig::sharpBonus / 100.0f));
            if (l_sharpBonus < 10)
                l_sharpBonus = 10;
            retValue += l_sharpBonus;
        }
        else
            retValue += NBConfig::sharpBonus;
    }

    if ((itemQuality & gEItemQuality_Blessed) == gEItemQuality_Blessed)
    {
        retValue += NBConfig::blessedBonus;
    }

    if ((itemQuality & gEItemQuality_Forged) == gEItemQuality_Forged)
    {
        retValue += NBConfig::forgedBonus;
    }

    return retValue;
}

GEInt OnPlayerGetDamage(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity, GEU32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);
    GEInt retValue = *(GEU32 *)a_iArgs;
    GEU32 itemQuality = *(GEU32 *)(a_iArgs + 0x4);

    if ((itemQuality & gEItemQuality_Worn) == gEItemQuality_Worn)
    {
        retValue = static_cast<GEInt>(
            retValue - (*(GEU32 *)a_iArgs * ((GEFloat)(100 - NBConfig::wornPercentageMalus) / 100.0f)));
    }

    if ((itemQuality & gEItemQuality_Sharp) == gEItemQuality_Sharp)
    {
        if (NBConfig::useSharpPercentage)
        {
            GEU32 l_sharpBonus = static_cast<GEU32>(*(GEU32 *)a_iArgs * ((GEFloat)NBConfig::sharpBonus / 100.0f));
            if (l_sharpBonus < 10)
                l_sharpBonus = 10;
            retValue += l_sharpBonus;
        }
        else
            retValue += NBConfig::sharpBonus;
    }

    if ((itemQuality & gEItemQuality_Blessed) == gEItemQuality_Blessed)
    {
        retValue += NBConfig::blessedBonus;
    }

    if ((itemQuality & gEItemQuality_Forged) == gEItemQuality_Forged)
    {
        retValue += NBConfig::forgedBonus;
    }

    return retValue;
}

static mCFunctionHook Hook_MagicProjectile;
GEInt MagicProjectile(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity, GEInt p_args)
{
    INIT_SCRIPT_EXT(Self, Other);
    Entity Spell = Self.Interaction.GetSpell();
    Entity Item = Self.Inventory.GetItemFromSlot(Self.Inventory.GetSlotFromStack(p_args));
    bCMatrix ItemPose = Item.GetPose();

    Entity Spawn = Spell.Magic.GetSpawn();
    Entity SpawnedProjectile = Entity::Spawn(Spawn.GetTemplate(), ItemPose);

    SpawnedProjectile.Interaction.SetOwner(Self);
    SpawnedProjectile.Interaction.SetSpell(Spell);
    SpawnedProjectile.CollisionShape.CreateShape(eECollisionShapeType_Point, eEShapeGroup_Projectile, bCVector(0, 0, 0),
                                                 bCVector(0, 0, 0));
    SpawnedProjectile.EnableCollisionWith(SpawnedProjectile, GEFalse);
    SpawnedProjectile.EnableCollisionWith(Self, GEFalse);

    bCVector Vec = SpawnedProjectile.GetPose().AccessZAxis();
    Vec.AccessY() += 0.01f;
    Vec.Normalize();

    // Free Aim Logic //
    if (Self.IsPlayer() && GETrue)
    {
        Vec = Self.Camera.GetScreenCenterPickPoint() - ItemPose.GetTranslation();
        Vec.Normalize();
        SpawnedProjectile.Projectile.SetTarget(None);
        // std::cout << "Impact Vec: x=" << Vec.GetX ( ) << ", y=" << Vec.GetY ( ) << ", z=" << Vec.GetZ ( ) << "\n";
    }
    else
    {
        SpawnedProjectile.Projectile.SetTarget(Other);
    }
    ////

    SpawnedProjectile.Projectile.AccessProperty<PSProjectile::PropertyPathStyle>() = gEProjectilePath_Line;
    if (Other != None && Self.Routine.GetProperty<PSRoutine::PropertyAction>() == gEAction_PowerCast)
    {
        SpawnedProjectile.Projectile.SetTarget(Other);
        SpawnedProjectile.Projectile.AccessProperty<PSProjectile::PropertyPathStyle>() = gEProjectilePath_Missile;
    }
    SpawnedProjectile.Projectile.AccessProperty<PSProjectile::PropertyTargetDirection>() = Vec;
    SpawnedProjectile.Projectile.Shoot();
    SpawnedProjectile.Damage.AccessProperty<PSDamage::PropertyManaUsed>() =
        Self.NPC.GetProperty<PSNpc::PropertyManaUsed>();
    return 1;
}

static mCFunctionHook Hook_OnTouch;
void OnTouch(eCEntity *p_entity, eCContactIterator *p_contactIterator)
{
    gCProjectile_PS *This = Hook_OnTouch.GetSelf<gCProjectile_PS *>();
    GEBool IsFlying = This->IsFlying();
    GEBool HasCollided = This->HasCollided();
    Hook_OnTouch.GetOriginalFunction (&OnTouch)(p_entity, p_contactIterator);
    if (HasCollided || !IsFlying)
        return;
    eCEntity *eCE = *(eCEntity **)((DWORD)This + 0xC);
    gCInteraction_PS *interaction = (gCInteraction_PS *)eCE->GetPropertySet(eEPropertySetType_Interaction);
    gCDamage_PS *damage = (gCDamage_PS *)eCE->GetPropertySet(eEPropertySetType_Damage);
    PSDamage damagePS = (PSDamage)damage;
    Entity owner = (Entity)interaction->GetOwner().GetEntity();
    for each (bCString entry in NBConfig::AOENames)
    {
        if (entry != "" && eCE->GetName().Contains(entry.GetText()))
        {
            This->AccessTouchBehavior() = gEProjectileTouchBehavior_KillSelf;
            /*bCVector loc = p_contactIterator->GetAvgCollisionPosition ( );
            //std::cout << "Location: x= " << loc.AccessX ( ) << "\ty= " << loc.AccessY ( )
            //    << "\tz= " << loc.AccessZ ( ) << "\n";
            bCMatrix pos = bCMatrix ( 0.0 );
            pos.SetToTranslation ( loc );

            Entity temp = Template ( "Spl_HeatWave" );
            if ( temp == None )
                return;
            Template templateSpawn = temp.Magic.GetSpawn ( ).GetTemplate ( );

            if ( !templateSpawn.IsValid ( ) )
                return;
            Entity spawn = Entity::Spawn ( templateSpawn , pos );
            PSDamage damagePS = damage;
            spawn.Interaction.SetOwner ( owner );
            spawn.Interaction.SetSpell ( interaction->GetSpell ( ).GetEntity ( ) );
            std::cout << "Name Spell = " << interaction->GetSpell ( ).GetEntity ( )->GetName() << "\n";
            GEInt damageAmount = damagePS.GetProperty<PSDamage::PropertyDamageAmount> ( );
            spawn.Damage.AccessProperty<PSDamage::PropertyDamageAmount> ( ) = damageAmount;
            std::cout << "DamageAmount in Ontouch = " << spawn.Damage.GetProperty<PSDamage::PropertyDamageAmount> ( ) <<
            "\n"; gEDamageType damageType = damagePS.GetProperty<PSDamage::PropertyDamageType> ( );
            spawn.Damage.AccessProperty<PSDamage::PropertyDamageType> ( ) = damageType;
            //std::cout << "DamageType = " << damageType << "\n";
            //eCE->EnableDeactivation ( GETrue ); //good one!*/
            bCVector loc = p_contactIterator->GetAvgCollisionPosition();
            bCMatrix pos = bCMatrix(0.0);
            pos.SetToTranslation(loc);
            Template templateSpawn = Template(eCE->GetTemplateID());
            if (!templateSpawn.IsValid())
            {
                print("templateSpawn invalid \n");
                return;
            }
            Entity spawn = Entity::Spawn(templateSpawn, pos);
            print("%s Created\n", spawn.GetName().GetText());
            spawn.Interaction.SetOwner(owner);
            spawn.Interaction.SetSpell((Entity)interaction->GetSpell().GetEntity());
            GEInt damageAmount = damagePS.GetProperty<PSDamage::PropertyDamageAmount>();
            if (owner.Routine.GetProperty<PSRoutine::PropertyAction>() == gEAction_PowerCast)
                damageAmount *= 2; // TODO: DamageManaMultiplier
            damageAmount = static_cast<GEInt>(static_cast<GEFloat>(damageAmount)
                                              * damagePS.GetProperty<PSDamage::PropertyDamageHitMultiplier>());
            spawn.Damage.AccessProperty<PSDamage::PropertyDamageAmount>() = damageAmount;
            GEInt manaUsed = damagePS.GetProperty<PSDamage::PropertyManaUsed>();
            spawn.Damage.AccessProperty<PSDamage::PropertyManaUsed>() = manaUsed;
            Entity entity = Entity(p_entity);
            DoAOEDamage(spawn, entity);
            spawn.Decay();
            eCE->EnablePicking(GEFalse, GEFalse); // Also really good!
            return;
        }
    }
}

void MagicPartyMemberRemoverNew(Entity &p_summoner, Template &p_monsterSummon, GEFloat multiplicator = 1.0f)
{
    auto partyMembers = p_summoner.Party.GetMembers(GEFalse);
    if (partyMembers.GetCount() == 0)
    {
        return;
    }

    Entity PartyMember = None;
    GEInt combinedLevel =
        static_cast<GEInt>(static_cast<Entity>(p_monsterSummon).NPC.GetProperty<PSNpc::PropertyLevelMax>()
                           * NBConfig::summoningLevelMultiplier * multiplicator);
    GEInt playerMaxMana = GetScriptAdminExt().CallScriptFromScript("GetManaPointsMax", &p_summoner, &None);

    for (GEInt iPartyMember = partyMembers.GetCount() - 1; iPartyMember >= 0; iPartyMember--)
    {
        PartyMember = (Entity)partyMembers[iPartyMember];
        gEPartyMemberType pMT = PartyMember.Party.GetProperty<PSParty::PropertyPartyMemberType>();

        if (pMT == gEPartyMemberType_Controlled)
        {
            bCUnicodeString partyMemberName = PartyMember.GetFocusName();
            eCLocString printText = eCLocString("GO_ControlDismiss");
            bCUnicodeString visualText = printText.GetString();
            visualText.Replace(L"$(name)", partyMemberName);
            gui2.PrintGameMessage(visualText, gEGameMessageType_Failure);
            PartyMember.Party.SetPartyLeader(None);
            if (PartyMember.Navigation.IsInProcessingRange())
            {
                PartyMember.Routine.ContinueRoutine();
            }
            continue;
        }

        if (pMT == gEPartyMemberType_Summoned)
        {
            if (PartyMember.Navigation.IsInProcessingRange() == GEFalse)
            {
                PartyMember.Routine.FullStop();
                PartyMember.Routine.SetTask("ZS_RagDollDead");

                PartyMember.Party.SetPartyLeader(None);
                PartyMember.Party.AccessProperty<PSParty::PropertyWaiting>() = GEFalse;

                PartyMember.Routine.AccessProperty<PSRoutine::PropertyAIMode>() = gEAIMode_Dead;
                continue;
            }

            // More Summons only for the Player now
            if (p_summoner.IsPlayer())
            {
                // Check if the Mana Potential is enough to hold on to this Monster-Summon
                GEInt currentLevelAdd = static_cast<GEInt>(PartyMember.NPC.GetProperty<PSNpc::PropertyLevelMax>()
                                                           * NBConfig::summoningLevelMultiplier);
                if ((combinedLevel + currentLevelAdd) <= playerMaxMana)
                {
                    combinedLevel += currentLevelAdd;
                    continue;
                }
            }

            PartyMember.Routine.FullStop();
            PartyMember.Routine.SetTask("ZS_RagDollDead");

            PartyMember.Party.SetPartyLeader(None);
            PartyMember.Party.AccessProperty<PSParty::PropertyWaiting>() = GEFalse;
        }
    }
}

DECLARE_SCRIPT(MagicSummonDemon)
{
    INIT_SCRIPT_EXT(Self, Other);
    UNREFERENCED_PARAMETER(a_iArgs);

    Template Spawn = "Demon";
    if (Self.IsPlayer() && GetScriptAdminExt().CallScriptFromScript("GetIntelligence", &Self, &None) >= 350)
    {
        Spawn = "DemonLord";
        if (!Spawn.IsValid())
        {
            Spawn = "Demon";
        }
    }

    MagicPartyMemberRemoverNew(Self, Spawn);

    PartyMonsterSpawn(Self, Spawn, 0, GEFalse);
    return 1;
}

DECLARE_SCRIPT(MagicSummonGoblin)
{
    INIT_SCRIPT_EXT(Self, Other);
    UNREFERENCED_PARAMETER(a_iArgs);

    Template Spawn = "Goblin";
    if (Self.IsPlayer() && GetScriptAdminExt().CallScriptFromScript("GetIntelligence", &Self, &None) >= 250)
    {
        Spawn = "GoblinWarrior";
        if (!Spawn.IsValid())
        {
            Spawn = "BlackGoblin";
        }
    }
    else if (Self.IsPlayer() && GetScriptAdminExt().CallScriptFromScript("GetIntelligence", &Self, &None) >= 150)
    {
        Spawn = "BlackGoblin";
    }
    MagicPartyMemberRemoverNew(Self, Spawn);

    PartyMonsterSpawn(Self, Spawn, 0, GEFalse);
    return 1;
}

DECLARE_SCRIPT(MagicSummonGolem)
{
    INIT_SCRIPT_EXT(Self, Other);
    UNREFERENCED_PARAMETER(a_iArgs);

    Template Spawn = "Golem";
    if (Self.IsPlayer() && GetScriptAdminExt().CallScriptFromScript("GetIntelligence", &Self, &None) >= 350)
    {
        Spawn = "GolemStrong";
        if (!Spawn.IsValid())
        {
            Spawn = "Golem";
        }
    }
    MagicPartyMemberRemoverNew(Self, Spawn);

    PartyMonsterSpawn(Self, Spawn, 0, GEFalse);
    return 1;
}

DECLARE_SCRIPT(MagicSummonSkeleton)
{
    INIT_SCRIPT_EXT(Self, Other);
    UNREFERENCED_PARAMETER(a_iArgs);

    Template Spawn = "Skeleton_Sword";
    if (Self.IsPlayer() && GetScriptAdminExt().CallScriptFromScript("GetIntelligence", &Self, &None) >= 250)
    {
        Spawn = "SkeletonWarrior";
        if (!Spawn.IsValid())
        {
            Spawn = "Skeleton_Sword";
        }
    }
    MagicPartyMemberRemoverNew(Self, Spawn);

    PartyMonsterSpawn(Self, Spawn, 0, GEFalse);
    return 1;
}

DECLARE_SCRIPT(MagicSummonCompanion)
{
    INIT_SCRIPT_EXT(Self, Other);
    UNREFERENCED_PARAMETER(a_iArgs);

    Template Spawn = Self.Interaction.GetSpell().Magic.GetSpawn().GetTemplate();
    GEFloat multiplicator = 1.0f;
    if (Self.IsPlayer() && GetScriptAdminExt().CallScriptFromScript("GetIntelligence", &Self, &None) >= 300)
    {
        multiplicator = 1.5f;
    }
    MagicPartyMemberRemoverNew(Self, Spawn, multiplicator);

    PartyMonsterSpawn(Self, Spawn, 0, GEFalse, multiplicator);
    return 1;
}

GEInt MagicSummonArmyOfDarkness(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                                GEInt p_args)
{
    INIT_SCRIPT_EXT(Self, Other);
    UNREFERENCED_PARAMETER(p_args);

    GEInt amount = 4;
    GEInt position = 0;
    using MSR = void (*)(Entity p_entity);
    MSR MonsterSpawnRemover = (MSR)RVA_ScriptGame(0x54770);
    MonsterSpawnRemover(Self);

    Template spawn1 = Template("ArmyOfDarknessSpawn1");
    Template spawn2 = Template("ArmyOfDarknessSpawn2");
    Template spawnBoss = Template("ArmyOfDarknessBoss");

    for (GEInt i = 0; i < amount; i++)
    {
        PartyMonsterSpawn(Self, spawn1, position++, GEFalse);
        PartyMonsterSpawn(Self, spawn2, position++, GEFalse);
    }
    PartyMonsterSpawn(Self, spawnBoss, position++, GEFalse);

    return 1;
}

GEInt GE_STDCALL CleanUpPlunderInv(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                                   GEI32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);
    UNREFERENCED_PARAMETER(a_iArgs);

    // Remove Weapons of Demons and Remove Weapons of Summoned Creatures!
    gEPartyMemberType selfPartyMemberType = Self.Party.GetProperty<PSParty::PropertyPartyMemberType>();
    if (Self.NPC.GetProperty<PSNpc::PropertySpecies>() == gESpecies_Demon
        || selfPartyMemberType == gEPartyMemberType_Summoned)
    {
        for (GEInt i = Self.Inventory.GetStackCount() - 1; i >= 0; i--)
        {
            if (Self.Inventory.UnEquipStack(i))
            {
                Self.Inventory.DeleteStack(i);
            }
        }
    }

    // Do not generate Items from Summoned Monsters!
    if (selfPartyMemberType == gEPartyMemberType_Summoned)
    {
        return GETrue;
    }

    Self.Inventory.GeneratePlunderInvFromTreasureSets();
    return GETrue;
}

DECLARE_SCRIPT(DropHandItems)
{
    INIT_SCRIPT_EXT(Self, Other);
    UNREFERENCED_PARAMETER(a_iArgs);

    GEBool decay = GEFalse;
    if (Self.Party.GetProperty<PSParty::PropertyPartyMemberType>() == gEPartyMemberType_Summoned
        || Self.NPC.GetProperty<PSNpc::PropertySpecies>() == gESpecies_Demon)
    {
        decay = GETrue;
    }
    Entity LeftHandItem = Self.Inventory.GetItemFromSlot(gESlot_LeftHand);
    Entity RightHandItem = Self.Inventory.GetItemFromSlot(gESlot_RightHand);

    if (GetScriptAdminExt().CallScriptFromScript("IsDroppableWeapon", &LeftHandItem, &None))
    {
        Entity droppedItem = Self.Inventory.DropItemsFromSlot(gESlot_LeftHand, 1);
        if (decay)
            droppedItem.Decay();
    }

    if (GetScriptAdminExt().CallScriptFromScript("IsDroppableWeapon", &RightHandItem, &None))
    {
        Entity droppedItem = Self.Inventory.DropItemsFromSlot(gESlot_RightHand, 1);
        if (decay)
            droppedItem.Decay();
    }

    Self.Inventory.HoldStacks(-1, -1);

    if (Self.IsPlayer())
    {
        using Func = void(Entity, GEInt, GEInt);
        Func *func = (Func *)RVA_ScriptGame(0x2e50);
        func(Entity::GetPlayer(), -1, -1);
    }
    return 1;
}

static mCFunctionHook Hook_ZS_RagdollDeadAddition;
GEBool ZS_RagdollDeadAddition(bTObjStack<gScriptRunTimeSingleState> &a_rRunTimeStack, gCScriptProcessingUnit *a_pSPU)
{
    /*INIT_SCRIPT_STATE ();
    gCScriptAdminExt& ScriptAdmin = GetScriptAdminExt ( );

    ScriptAdmin.CallScriptFromScript ( "SetHitPoints" , &SelfEntity , &None, 0 );

    if ( SelfEntity.Routine.AIMode == gEAIMode_Down ) {
        SelfEntity.Routine.SetTask ( "ZS_Dead" );
        return GETrue;
    }

    if ( ScriptAdmin.CallScriptFromScript ( "IsHumanoid" , &SelfEntity , &None ) ) {
        SelfEntity.StartSaySVM ( SelfEntity , "DEAD" , GEFalse );
    }
    else {
        bCString effectName = "";
        if ( SelfEntity.Animation.GetSkeletonName ( effectName ) ) {
            EffectSystem::StartEffect ( "eff_creature_" + effectName + "_die_01" , SelfEntity );
        }
    }

    Entity attacker = SelfEntity.NPC.GetCurrentAttacker ( );

    ScriptAdmin.CallScriptFromScript ( "Kill" , &attacker , &SelfEntity );

    SelfEntity.CharacterMovement.SetMovementMode ( gECharMovementMode_RagDollDead );


    //END
    return GETrue;*/
    GEBool retValue = Hook_ZS_RagdollDeadAddition.GetOriginalFunction(&ZS_RagdollDeadAddition)(a_rRunTimeStack, a_pSPU);
    Entity Self = (Entity)a_pSPU->GetSelfEntity();
    if (Self.Party.GetProperty<PSParty::PropertyPartyMemberType>() == gEPartyMemberType_Summoned)
    {
        VanishEntity(Self);
    }
    return retValue;
}

// TODO
/*
GEBool sAICombatMoveStart(gCScriptProcessingUnit::sAICombatMoveInstr_Args *p_args, gCScriptProcessingUnit *p_SPU)
{
    // TODO Reverse Engineer it!
    /*p_SPU->SetSelfEntity(p_args->SelfEntity);
    p_SPU->SetOtherEntity(p_args->TargetEntity);
    //*reinterpret_cast<gEAction *>(p_SPU + 0x154) = p_args->Action;
    p_SPU->m_fInstrAction = p_args->Action;


    if (!p_args->SelfEntity)
        return GEFalse;

    p_SPU->m_SelfMovementPS =
        GetPropertySet<gCCharacterMovement_PS>(p_args->SelfEntity, eEPropertySetType_CharacterMovement);

    if (!p_SPU->m_SelfMovementPS)
        return GEFalse;

    bCVector directionVec;
    directionVec.Clear();
    if (!p_SPU->m_OtherEntity.GetEntity())
    {
        auto selfEntity = p_SPU->m_SelfEntity.GetEntity();
        gCCharacterControl_PS* selfCCPtr = GetPropertySet<gCCharacterControl_PS>(selfEntity,
    eEPropertySetType_CharacterControl); if (!selfCCPtr)
        {
            directionVec = selfEntity->GetWorldMatrix().GetZAxis();
        }
        else
        {
            //desired direction of character controll?
            directionVec = *reinterpret_cast<bCVector *>(reinterpret_cast<uintptr_t>(selfCCPtr) + 0x60);
            if (directionVec.HasZeroMagnitude2D(bCVector::bECoordinate_Y))
            {
                directionVec = selfEntity->GetWorldMatrix().GetZAxis();
            }
        }
    }
    else
    {
        directionVec = p_SPU->GetOtherEntity()->GetWorldPosition() - p_SPU->GetSelfEntity()->GetWorldPosition();
    }

    p_SPU->m_DirectionVec = directionVec;

    if (p_SPU->m_SelfMovementPS->GetMovementIsControledByPlayer())
    {
        gCCameraAI_PS *cameraPtr = gCSession::GetInstance().GetCamera_PS();
        if (cameraPtr)
        {
            p_SPU->m_DirectionVec = cameraPtr->GetEntity()->GetAtVector();
        }
    }*/

// Old variant, by rewriting whole CombatMoveStart
/*
static mCFunctionHook Hook_sAICombatMoveStart;
GEBool sAICombatMoveStart(gCScriptProcessingUnit::sAICombatMoveInstr_Args *p_args, gCScriptProcessingUnit *p_SPU)
{
    if (p_args->Action != gEAction_Evade)
    {
        return Hook_sAICombatMoveStart.GetOriginalFunction(&sAICombatMoveStart)(p_args, p_SPU);
    }

    p_SPU->m_Speaker.SetEntity(p_args->SelfEntity);
    p_SPU->m_CombatTarget.SetEntity(p_args->TargetEntity);
    p_SPU->m_fInstrAction = p_args->Action;

    if (!p_args->SelfEntity)
    {
        println("Missing Self Entity");
        return GEFalse;
    }

    p_SPU->m_SelfMovementPS =
        GetPropertySet<gCCharacterMovement_PS>(p_args->SelfEntity, eEPropertySetType_CharacterMovement);

    if (!p_SPU->m_SelfMovementPS)
    {
        println("Missing CharacterMovementPS");
        return GEFalse;
    }

    p_SPU->m_DirectionVec.Normalize();

    p_SPU->m_SelfNavigationPS = GetPropertySet<gCNavigation_PS>(p_SPU->GetSelfEntity(), eEPropertySetType_Navigation);
    if (p_SPU->m_SelfNavigationPS)
        p_SPU->m_SelfNavigationPS->SetCurrentAniDirection(gEDirection_Fwd);

    const GEFloat EvadeLength = 250;
    bCString aniName;
    auto Self = p_SPU->GetSelfEntity();

    if (p_args->PhaseName.Contains("Right"))
    {
        if (p_args->PhaseName == "Right_Raise")
        {
            aniName = "Hero_EvadeRight_Raise";
        }

        else if (p_args->PhaseName == "Right_Recover")
        {
            aniName = "Hero_EvadeRight_Recover";
        }

        else
        {
            aniName = "Hero_EvadeRight_Hit";
        }

        p_SPU->m_DirectionVec = Self->GetWorldMatrix().GetXAxis().GetNormalized();
    }
    else if (p_args->PhaseName.Contains("Left"))
    {
        if (p_args->PhaseName == "Left_Raise")
        {
            aniName = "Hero_EvadeLeft_Raise";
        }

        else if (p_args->PhaseName == "Left_Recover")
        {
            aniName = "Hero_EvadeLeft_Recover";
        }

        else
        {
            aniName = "Hero_EvadeLeft_Hit";
        }

        p_SPU->m_DirectionVec = -Self->GetWorldMatrix().GetXAxis().GetNormalized();
    }
    else
    {
        if (p_args->PhaseName == "Raise")
        {
            aniName = "Hero_EvadeBack_Raise";
            // p_SPU->m_DirectionVec = -Self->GetWorldMatrix().GetXAxis() * EvadeLength;
        }

        else if (p_args->PhaseName == "Recover")
        {
            aniName = "Hero_EvadeBack_Recover";
            // p_SPU->m_DirectionVec = Self->GetWorldMatrix().GetXAxis() * EvadeLength;
        }

        else
        {
            aniName = "Hero_EvadeBack_Hit";
            // p_SPU->m_DirectionVec = -Self->GetWorldMatrix().GetZAxis() * EvadeLength;
        }

        p_SPU->m_DirectionVec = -Self->GetWorldMatrix().GetZAxis().GetNormalized();
    }

    p_SPU->m_fAniString = aniName;
    p_SPU->m_SelfAnimationPS =
        GetPropertySet<eCVisualAnimation_PS>(p_SPU->GetSelfEntity(), eEPropertySetType_Animation);

    if (!p_SPU->m_SelfAnimationPS)
    {
        println("Missing Visual Animation");
        return GEFalse;
    }

    // const char *nativAniFileExt = reinterpret_cast<const char *>(RVA_Engine(0x602ef8));
    p_SPU->m_fAniString += eCResourceAnimationMotion_PS().GetNativeFileExt();

    eCAnimationAdmin *animationAdmin = FindModule<eCAnimationAdmin>();
    if (!animationAdmin)
    {
        println("Missing animationAdmin");
        return GEFalse;
    }

    eCResourceDataEntity *resDataEntity = NULL;
    GEBool isAniMissed = animationAdmin->IsAnimationMissed(aniName);
    if (!isAniMissed)
    {
        resDataEntity = animationAdmin->QueryMotionDataEntity(aniName, eEResourceCacheBehavior_Immediate);
    }

    if (isAniMissed || !resDataEntity)
    {
        animationAdmin->AddMissingAnimation(aniName);
        println(aniName);
        println("Ani for Evading is missing");
        return GEFalse;
    }

    gCNPC_PS *selfNPCPS = GetPropertySet<gCNPC_PS>(p_SPU->GetSelfEntity(), eEPropertySetType_NPC);
    bCString resDataEntityFileName = resDataEntity->GetFileName();
    if (selfNPCPS)
    {
        selfNPCPS->SetCurrentMovementAni(resDataEntityFileName);
    }

    eCWrapper_emfx2Actor *selfActor = p_SPU->m_SelfAnimationPS->GetActor();
    if (!selfActor)
    {
        println("Missing Actor");
        return GEFalse;
    }

    GEFloat extroBlending = selfActor->GetExtroBlending(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst);
    selfActor->SetIntroExtroBlending(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst, resDataEntityFileName);

    if (p_SPU->m_SelfAnimationPS->HasMotion(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst))
    {
        if (selfActor->IsMotionRunning(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst))
        {
            if (p_SPU->m_fAniString.Contains("_Begin_"))
            {
                selfActor->SwitchFadeOut(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst, GEFalse, 0.1f);
            }
            else
            {
                GEFloat introBlending = selfActor->GetIntroBlending(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst);
                selfActor->SwitchFadeOut(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst, GEFalse,
                                         Min(introBlending, extroBlending));
            }
        }
        else
        {
            selfActor->SwitchFadeOut(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst, GEFalse, extroBlending);
        }

        p_SPU->m_SelfAnimationPS->StopMotion(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst, 0.0f);
        selfActor->SetTargetWeight(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst, 0.0f);
        p_SPU->m_SelfAnimationPS->SetMotionOwner(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst,
                                                  static_cast<eCVisualAnimation_PS::eSMotionDesc::eEMotionOwner>(2));
    }

    if (p_SPU->m_SelfAnimationPS->HasMotion(eCWrapper_emfx2Actor::eEMotionType_PrimarySecond))
    {
        p_SPU->m_SelfAnimationPS->StopMotion(eCWrapper_emfx2Actor::eEMotionType_PrimarySecond, 0.0f);
        selfActor->SetTargetWeight(eCWrapper_emfx2Actor::eEMotionType_PrimarySecond, 0.0f);
        p_SPU->m_SelfAnimationPS->SetMotionOwner(eCWrapper_emfx2Actor::eEMotionType_PrimarySecond,
                                                  static_cast<eCVisualAnimation_PS::eSMotionDesc::eEMotionOwner>(2));
    }

    if (p_SPU->m_SelfAnimationPS->HasMotion(eCWrapper_emfx2Actor::eEMotionType_PrimaryThird))
    {
        p_SPU->m_SelfAnimationPS->StopMotion(eCWrapper_emfx2Actor::eEMotionType_PrimaryThird, 0.0f);
        selfActor->SetTargetWeight(eCWrapper_emfx2Actor::eEMotionType_PrimaryThird, 0.0f);
        p_SPU->m_SelfAnimationPS->SetMotionOwner(eCWrapper_emfx2Actor::eEMotionType_PrimaryThird,
                                                  static_cast<eCVisualAnimation_PS::eSMotionDesc::eEMotionOwner>(2));
    }

    if (p_SPU->m_SelfAnimationPS->HasMotion(eCWrapper_emfx2Actor::eEMotionType_PrimaryFourth))
    {
        p_SPU->m_SelfAnimationPS->StopMotion(eCWrapper_emfx2Actor::eEMotionType_PrimaryFourth, 0.0f);
        selfActor->SetTargetWeight(eCWrapper_emfx2Actor::eEMotionType_PrimaryFourth, 0.0f);
        p_SPU->m_SelfAnimationPS->SetMotionOwner(eCWrapper_emfx2Actor::eEMotionType_PrimaryFourth,
                                                  static_cast<eCVisualAnimation_PS::eSMotionDesc::eEMotionOwner>(2));
    }

    p_SPU->m_SelfAnimationPS->SetMotion(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst, resDataEntity);

    p_SPU->m_fAniSpeedScale = p_args->AniSpeedScale;
    p_SPU->m_sMotionDesc = eCWrapper_emfx2Motion::eSMotionDesc();
    p_SPU->m_sMotionDesc.m_fAniSpeedScale = p_SPU->m_fAniSpeedScale;
    p_SPU->m_sMotionDesc.m_fFirst = 0.0f;
    p_SPU->m_sMotionDesc.m_fSixth = 0.0f;
    p_SPU->m_sMotionDesc.m_fSecond = 0;
    p_SPU->m_sMotionDesc.m_fFourth = 1;
    p_SPU->m_sMotionDesc.m_fUnknown1 = 1.0f;
    p_SPU->m_sMotionDesc.m_fUnknown2 = 1;

    if (p_SPU->m_SelfAnimationPS->HasMotion(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst))
    {
        p_SPU->m_SelfAnimationPS->PlayMotion(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst, &p_SPU->m_sMotionDesc);
    }

    GEFloat maxTime = selfActor->GetMaxTime(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst);
    GEFloat aniLength = maxTime / p_args->AniSpeedScale;
    eCEntityProxy target;
    target.SetEntity(p_SPU->GetOtherEntity());

    if (aniLength != 0.0f)
    {
        if (p_SPU->m_fAniString.Contains("Hit"))
        {
            p_SPU->m_DirectionVec *= EvadeLength / aniLength;
            p_SPU->m_SelfMovementPS->EnableCombatMovementFromSPU(GETrue, target, p_SPU->m_DirectionVec);
        }
    }

    p_SPU->m_SelfMovementPS->EnableMovementFromSPU(GEFalse);
    if (p_SPU->m_SelfNavigationPS)
    {
        p_SPU->m_SelfNavigationPS->UpdateInteractObject();
        p_SPU->m_SelfNavigationPS->SetIsOnDestination(GEFalse);
    }

    if (resDataEntity)
    {
        resDataEntity->ReleaseReference();
        resDataEntity = NULL;
    }

    p_SPU->m_SelfAnimationPS->SetMotionOwner(eCWrapper_emfx2Actor::eEMotionType_PrimaryFirst,
                                              static_cast<eCVisualAnimation_PS::eSMotionDesc::eEMotionOwner>(5));

    return GETrue;
}*/

static mCFunctionHook Hook_DoLogicalDamageEvade;
DECLARE_SCRIPT(DoLogicalDamageEvade)
{
    INIT_SCRIPT_EXT(Damager, Victim);

    if (Victim.Routine.GetProperty<PSRoutine::PropertyAction>() == gEAction_Evade)
    {
        auto characterNPC = GetPropertySet<gCNPC_PS>(Victim.GetInstance(), eEPropertySetType_NPC);
        if (characterNPC)
        {
            println(characterNPC->GetCurrentMovementAni());
            if (characterNPC->GetCurrentMovementAni().Contains("Hit"))
            {
                return GEFalse;
            }
        }
    }

    return Hook_DoLogicalDamageEvade.GetOriginalFunction(&DoLogicalDamageEvade)(a_pSPU, a_pSelfEntity, a_pOtherEntity,
                                                                                a_iArgs);
}

static mCFunctionHook Hook_FAI_Active_HardCodeAttacks;
DECLARE_SCRIPT(FAI_Active_NB)
{
    INIT_SCRIPT_EXT(Self, Victim);

    gEAction returnedAction = static_cast<gEAction>(Hook_FAI_Active_HardCodeAttacks.GetOriginalFunction(&FAI_Active_NB)(
        a_pSPU, a_pSelfEntity, a_pOtherEntity, a_iArgs));

    GEFloat distanceToTarget = Self.GetDistanceTo(Victim);

    // Overwrite Fwd for long distances!
    if (returnedAction == gEAction_Fwd && distanceToTarget >= 450.0f)
    {
        // Let NPCs sprint forward!
        if (NBConfig::enableNPCSprint
            && (Self.NPC.GetProperty<PSNpc::PropertySpecies>() != gESpecies_Zombie || NBConfig::zombiesCanSprint))
        {
            GEDouble staminaPercentage =
                (GEDouble)Self.DamageReceiver.GetProperty<PSDamageReceiver::PropertyStaminaPoints>()
                / (GEDouble)Self.DamageReceiver.GetProperty<PSDamageReceiver::PropertyStaminaPointsMax>();
            if (staminaPercentage >= 0.2)
                Self.SetMovementMode(gECharMovementMode_Sprint);
        }

        return returnedAction;
    }

    // Handle the hardcore attack next!

    // Always attack then with patched instructions!
    if (NBConfig::useHardCoreAttacks)
    {
        return returnedAction;
    }

    // NPCs can attack each others fully
    if (Victim != Entity::GetPlayer() || Victim == None)
    {
        return returnedAction;
    }

    gEAniState victimAniState = Victim.Routine.GetProperty<PSRoutine::PropertyAniState>();
    gEAction victimAction = Victim.Routine.GetProperty<PSRoutine::PropertyAction>();

    // Do default behaviour on hard difficulty with adjusted hit registrations
    if (Entity::GetCurrentDifficulty() == EDifficulty_Hard)
    {
        if (victimAniState == gEAniState_SitKnockDown)
        {
            if (distanceToTarget < 350)
            {
                return gEAction_Back;
            }
            else if (distanceToTarget > 400)
            {
                return gEAction_Fwd;
            }
            return gEAction_Wait;
        }
    }

    // Else be less aggressive against Player!
    // gEAction_SitKnockDown should be enought, since it is applied when in ZS_SitKnockDown state
    else if (victimAction == gEAction_SitKnockDown || victimAniState == gEAniState_SitKnockDown
             || victimAniState == gEAniState_LieKnockDown)
    {
        if (distanceToTarget < 400)
        {
            return gEAction_Back;
        }
        else if (distanceToTarget > 500)
        {
            return gEAction_Fwd;
        }
        return gEAction_Wait;
    }

    // Default function (with patched checks)
    return returnedAction;
}

// Has issues when hooking sadly!
/*static mCFunctionHook Hook_Entity_AttachTo;
GEBool GE_STDCALL Entity_AttachTo(LPVOID *a_peCEntity)
{
    Entity *_this = Hook_Entity_AttachTo.GetSelf<Entity *>();
    if (_this == NULL)
    {
        return Hook_Entity_AttachTo.GetOriginalFunction(&Entity_AttachTo)(a_peCEntity);
    }

    if (a_peCEntity == NULL)
    {
        return Hook_Entity_AttachTo.GetOriginalFunction(&Entity_AttachTo)(a_peCEntity);
    }

    // Sometimes when killing asynchronously the Entity, the Objects vtable is getting replaced with bCObjectBase
    // vtable!
    uintptr_t m_fFunctionPtr = *(uintptr_t *)a_peCEntity + 0xA0;
    if (m_fFunctionPtr == NULL || *(uintptr_t *)m_fFunctionPtr == NULL)
    {
        println("eCEntity::IsKilled() method broken, trying to repair...");
        memset(_this, 0, sizeof(Entity));
        return GETrue;
    }

    return Hook_Entity_AttachTo.GetOriginalFunction(&Entity_AttachTo)(a_peCEntity);
}*/

static mCFunctionHook Hook_OnPlayerSecondaryAction_NB;
DECLARE_SCRIPT(OnPlayerSecondaryAction_NB)
{
    INIT_SCRIPT_EXT(Self, Other);

    if (Self.Routine.GetCurrentTask() != "PS_Melee")
    {
        return Hook_OnPlayerSecondaryAction_NB.GetOriginalFunction(&OnPlayerSecondaryAction_NB)(
            a_pSPU, a_pSelfEntity, a_pOtherEntity, a_iArgs);
    }

    // Usually it would be better to add an action instead of hard setting states,
    // for the current task loop to handle, but we want less hooks :)

    static eCKeyboard &keyboard = eCApplication::GetInstance().GetKeyboard();
    // Maps SessionKeys to physical Keyboard Keys
    // Protected constructor workaround
    static gCSessionKeys sessionKeys = gCSessionKeys();
    sessionKeys = gCSession::GetInstance().GetSessionKeys();

    eCPhysicalKey *shiftKey = sessionKeys.GetAssignedKey(gESessionKey_Walk, 0);
    eCPhysicalKey *shiftKeyAlt = sessionKeys.GetAssignedKey(gESessionKey_Walk, 1);

    GEBool runKeyPressed = keyboard.KeyPressed(shiftKey->m_enuKeyboardStateOffset)
                        || keyboard.KeyPressed(shiftKeyAlt->m_enuKeyboardStateOffset);

    // Tries to parry!
    if (runKeyPressed)
    {
        // Do to allow spamming Parry or canceling forced recovery in other states!
        // if (Self.Routine.GetProperty<PSRoutine::PropertyAction>() == gEAction_Parry
        if (Self.Routine.GetCurrentState() != "PS_Melee_Loop")
        {
            return GETrue;
        }

        if (!Self.CharacterControl.GetProperty<PSCharacterControl::PropertyIsPressed>()
            && Self.CharacterControl.GetProperty<PSCharacterControl::PropertyDurationPressedMSecs>() <= 200)
        {
            gESpecies selfSpecies = Self.NPC.GetProperty<PSNpc::PropertySpecies>();
            if (selfSpecies == gESpecies_Orc || selfSpecies == gESpecies_Human)
            {
                GEBool *paradeBool = reinterpret_cast<GEBool *>(RVA_ScriptGame(0x118b41));
                *paradeBool = GEFalse;
                SetParadeMode(Self, GEFalse);
                Self.Routine.SetState("NB_Melee_Parry");
            }
        }

        return GETrue;
    }

    return Hook_OnPlayerSecondaryAction_NB.GetOriginalFunction(&OnPlayerSecondaryAction_NB)(a_pSPU, a_pSelfEntity,
                                                                                            a_pOtherEntity, a_iArgs);
}

void HookFunctions()
{
    Hook_OnPlayerSecondaryAction_NB.Hook(GetScriptAdminExt().GetScript("OnPlayerSecondaryAction")->m_funcScript,
                                         &OnPlayerSecondaryAction_NB);

    // Old variant
    // Hook_sAICombatMoveStart.Hook(RVA_Game(0x16abb0), &sAICombatMoveStart, mCBaseHook::mEHookType_ThisCall);

    Hook_DoLogicalDamageEvade.Hook(GetScriptAdminExt().GetScript("DoLogicalDamage")->m_funcScript,
                                   &DoLogicalDamageEvade);

    // Bugs regarding Debug built and using items, which changed stats of Player
    // Hook_Entity_AttachTo.Prepare(RVA_Script(0x13e80), &Entity_AttachTo, mCBaseHook::mEHookType_ThisCall).Hook();

    Hook_FAI_Active_HardCodeAttacks.Hook(GetScriptAdminExt().GetScript("FAI_Active")->m_funcScript, &FAI_Active_NB);

    if (NBConfig::enableNewMagicAiming)
    {
        Hook_MagicProjectile.Prepare(RVA_ScriptGame(0x52db0), &MagicProjectile).Hook();
    }

    if (NBConfig::enableAOEDamage)
    {
        Hook_OnTouch.Prepare(RVA_Game(0x152650), &OnTouch, mCBaseHook::mEHookType_ThisCall).Hook();
    }

    if (NBConfig::vanishSummons)
    {
        Hook_ZS_RagdollDeadAddition.Prepare(RVA_ScriptGame(0x1c4a0), &ZS_RagdollDeadAddition).Hook();
    }

    if (NBConfig::newSummoning)
    {
        static mCFunctionHook Hook_MagicSummonDemon;
        Hook_MagicSummonDemon.Hook(GetScriptAdminExt().GetScript("MagicSummonDemon")->m_funcScript, &MagicSummonDemon);
        static mCFunctionHook Hook_MagicSummonGolem;
        Hook_MagicSummonDemon.Hook(GetScriptAdminExt().GetScript("MagicSummonGolem")->m_funcScript, &MagicSummonGolem);
        static mCFunctionHook Hook_MagicSummonGoblin;
        Hook_MagicSummonDemon.Hook(GetScriptAdminExt().GetScript("MagicSummonGoblin")->m_funcScript,
                                   &MagicSummonGoblin);
        static mCFunctionHook Hook_MagicSummonSkeleton;
        Hook_MagicSummonDemon.Hook(GetScriptAdminExt().GetScript("MagicSummonSkeleton")->m_funcScript,
                                   &MagicSummonSkeleton);
        static mCFunctionHook Hook_MagicSummonCompanion;
        Hook_MagicSummonDemon.Hook(GetScriptAdminExt().GetScript("MagicSummonCompanion")->m_funcScript,
                                   &MagicSummonCompanion);
        static mCFunctionHook Hook_MagicSummonArmyOfDarkness;
        Hook_MagicSummonArmyOfDarkness.Hook(GetScriptAdminExt().GetScript("MagicSummonArmyOfDarkness")->m_funcScript,
                                            &MagicSummonArmyOfDarkness);
    }

    static mCFunctionHook Hook_CleanUpPlunderInv;
    Hook_CleanUpPlunderInv.Hook(GetScriptAdminExt().GetScript("CleanUpPlunderInv")->m_funcScript, &CleanUpPlunderInv);

    static mCFunctionHook Hook_DropHandItems;
    Hook_DropHandItems.Hook(GetScriptAdminExt().GetScript("DropHandItems")->m_funcScript, &DropHandItems);

    static mCFunctionHook Hook_CanBurn;
    GetScriptAdmin().LoadScriptDLL("Script_G3Fixes.dll");
    if (!GetScriptAdmin().IsScriptDLLLoaded("Script_G3Fixes.dll") || NBConfig::useNewBalanceMagicWeapon)
    {
        Hook_CanBurn.Hook(GetScriptAdminExt().GetScript("CanBurn")->m_funcScript, &CanBurn,
                          mCBaseHook::mEHookType_OnlyStack);
        Hook_CanFreeze.Hook(GetScriptAdminExt().GetScript("CanFreeze")->m_funcScript, &CanFreeze,
                            mCBaseHook::mEHookType_OnlyStack);
    }
    else
    {
        Hook_CanFreeze.Hook(GetScriptAdminExt().GetScript("CanFreeze")->m_funcScript, &CanFreezeAddition,
                            mCBaseHook::mEHookType_OnlyStack);
    }

    static mCFunctionHook Hook_MagicPoison;
    Hook_MagicPoison.Hook(GetScriptAdminExt().GetScript("MagicPoison")->m_funcScript, &MagicPoison);

    static mCFunctionHook Hook_CanBePoisoned;
    Hook_CanBePoisoned.Hook(GetScriptAdminExt().GetScript("CanBePoisoned")->m_funcScript, &CanBePoisoned);

    Hook_AddHitPoints.Prepare(RVA_ScriptGame(0x35b50), &AddHitPoints).Hook();
    Hook_OnTick.Prepare(RVA_ScriptGame(0xb0ef0), &OnTick).Hook();

    static mCFunctionHook Hook_CanParade;
    GetScriptAdmin().LoadScriptDLL("Script_OptionalGuard.dll");
    if (!GetScriptAdmin().IsScriptDLLLoaded("Script_OptionalGuard.dll"))
    {
        Hook_CanParade.Prepare(RVA_ScriptGame(0xd480), &CanParade, mCBaseHook::mEHookType_OnlyStack).Hook();
    }

    static mCFunctionHook Hook_IsEvil;
    Hook_IsEvil.Hook(GetScriptAdminExt().GetScript("IsEvil")->m_funcScript, &IsEvil);

    Hook_GetAnimationSpeedModifier
        .Prepare(RVA_ScriptGame(0x42a0), &GetAnimationSpeedModifier, mCBaseHook::mEHookType_Mixed, mERegisterType_Eax)
        .Hook();

    static mCFunctionHook Hook_SpeciesRightHand;
    Hook_SpeciesRightHand.Prepare(RVA_ScriptGame(0xb200), &speciesRightHand).Hook();

    if (NBConfig::enableNewTransformation)
    {
        Hook_MagicTransform.Hook(GetScriptAdminExt().GetScript("MagicTransform")->m_funcScript, &MagicTransform,
                                 mCBaseHook::mEHookType_OnlyStack);

        Hook_StartTransform.Prepare(RVA_Script(0x1afe0), &StartTransform, mCBaseHook::mEHookType_ThisCall).Hook();
    }

    Hook_MagicSleep.Hook(GetScriptAdminExt().GetScript("MagicSleep")->m_funcScript, &MagicSleep);

    static mCFunctionHook Hook_OnPowerAim_Loop;
    Hook_OnPowerAim_Loop.Prepare(RVA_ScriptGame(0x84b90), &OnPowerAim_Loop).Hook();

    static mCFunctionHook Hook_UpdateHitPointsOnTick;
    Hook_UpdateHitPointsOnTick.Prepare(RVA_ScriptGame(0xb0360), &UpdateHitPointsOnTick).Hook();

    static mCFunctionHook Hook_GetProtection;
    Hook_GetProtection.Hook(GetScriptAdminExt().GetScript("GetProtection")->m_funcScript, &GetProtection);

    static mCFunctionHook Hook_GetProtectionForHUD;
    Hook_GetProtection.Hook(GetScriptAdminExt().GetScript("GetProtectionForHUD")->m_funcScript, &GetProtectionHUD);

    if (NBConfig::useAlwaysMaxLevel)
        Hook_GetCurrentLevel.Hook(GetScriptAdminExt().GetScript("GetCurrentLevel")->m_funcScript, &GetCurrentLevel);

    if (NBConfig::useNewStaminaRecovery)
    {
        Hook_AddStaminaPoints.Hook(GetScriptAdminExt().GetScript("AddStaminaPoints")->m_funcScript, &AddStaminaPoints);

        Hook_StaminaUpdateOnTick
            .Prepare(RVA_ScriptGame(0xb0520), &StaminaUpdateOnTick, mCBaseHook::mEHookType_OnlyStack)
            .Hook();
    }

    Hook_GetAttituteSummons.Hook(GetScriptAdminExt().GetScript("GetAttitude")->m_funcScript, &GetAttitudeSummons);

    static mCFunctionHook Hook_GetQualityBonus;
    static mCFunctionHook Hook_OnPlayerGetDamage;
    Hook_GetQualityBonus.Hook(GetScriptAdminExt().GetScript("GetQualityBonus")->m_funcScript, &GetQualityBonus);
    Hook_GetQualityBonus.Hook(GetScriptAdminExt().GetScript("OnPlayerGetDamage")->m_funcScript, &OnPlayerGetDamage);

    static mCFunctionHook Hook_CanParadeMoveOf;
    Hook_CanParadeMoveOf.Hook(GetScriptAdminExt().GetScript("CanParadeMoveOf")->m_funcScript, &CanParadeMoveOf);

    // Hook_GetMaxLevel.Hook ( GetScriptAdminExt ( ).GetScript ( "GetLevelMax" )->m_funcScript , &GetMaxLevel );

    // TODO: Bring that shit on!
    // Hook_PS_Melee_Attack
    //	.Prepare ( RVA_ScriptGame ( 0x7eee0 ) , &PS_Melee_Attack )
    //	.Hook ( );

    // Hook_PS_Melee_PowerAttack
    //	.Prepare ( RVA_ScriptGame ( 0x7f420 ) , &PS_Melee_PowerAttack )
    //	.Hook ( );
    // Hook_GetAniName
    //	.Prepare ( RVA_Game ( 0x16f840 ) , &GetAniName , mCBaseHook::mEHookType_ThisCall )
    //	.Hook ( );
}

/*ME_DEFINE_AND_REGISTER_SCRIPT ( MagicSummonWolfPack )
{
    UNREFERENCED_PARAMETER ( a_iArgs );
    INIT_SCRIPT_EXT ( SelfEntity , Target );

    bCMatrix Pos = Target.GetPose ( );

    auto Spell = Target.Interaction.GetSpell ( );

    for ( GEInt x = 0; x < 5; x++ )
    {
        auto Spawned = Entity::Spawn ( Template ( "IceWolf" ) , Pos );
        bCMatrix NewPos;
        if ( Spawned.FindSpawnPose ( NewPos , Target , true , x + 1 ) )
        {
            Spawned.MoveTo ( NewPos );
            Spawned.Effect.StartEffect ( Spell.Magic.EffectTargetCast , GEFalse );
            Target.Party.Add ( Spawned );
            Spawned.Party.Waiting = GEFalse;
            Spawned.Party.PartyMemberType = gEPartyMemberType::gEPartyMemberType_Summoned;
            Spawned.Routine.FullStop ( );
            Spawned.Routine.SetTask ( "ZS_FollowPlayer" );
            Spawned.Dialog.PartyEnabled = GETrue;
        }
        else
        {
            Spawned.Kill ( );
            Spawned.Decay ( );
        }
    }

    return 0;
}*/
