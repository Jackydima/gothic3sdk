#include "AssessHit.h"

enum gEHitForce
{
    gEHitForce_Minimal = 0,
    gEHitForce_Normal = 1,
    gEHitForce_Heavy = 2
};

gEAction GE_STDCALL AssessHitNew(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                                 GEU32 a_iArgs);

gEAction GE_STDCALL AssessHitOld(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                                 GEU32 a_iArgs);

gEAction GE_STDCALL AssessHit(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                              GEU32 a_iArgs)
{
    if (NBConfig::UseNewDamageCalculation)
    {
        return AssessHitNew(a_pSPU, a_pSelfEntity, a_pOtherEntity, a_iArgs);
    }

    return AssessHitOld(a_pSPU, a_pSelfEntity, a_pOtherEntity, a_iArgs);
}

// wird aufgerufen von DoLogicalDamage
gEAction GE_STDCALL AssessHitOld(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                                 GEU32 a_iArgs)
{
    INIT_SCRIPT_EXT(Victim, Damager);
    UNREFERENCED_PARAMETER(a_iArgs);

    if (Victim == None || Damager == None)
        return gEAction_None;

    gCScriptAdmin &ScriptAdmin = GetScriptAdmin();
    // println("\n--------------------------------------------------------------\n");

    //
    // Schritt 1: Basisschaden
    //

    // Damager would be the weapon / projectile and DamagerOwner the attacking player / NPC.
    Entity DamagerOwner = Damager.Interaction.GetOwner();
    if (DamagerOwner == None && Damager.Navigation.IsValid())
    {
        DamagerOwner = Damager;
    }
    gEAction DamagerOwnerAction = DamagerOwner.Routine.GetProperty<PSRoutine::PropertyAction>();
    gEAction VictimAction = Victim.Routine.GetProperty<PSRoutine::PropertyAction>();

    Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageAmount>() = 0;
    Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageType>() = gEDamageType_Blade; // Default!
    Victim.NPC.SetLastAttacker(Victim.NPC.GetCurrentAttacker());
    Victim.NPC.SetCurrentAttacker(DamagerOwner);

    if (!Victim.NPC.IsFrozen() // Frozen Victim cannot parry!, should also never happen with state checks!
        && !Damager.Projectile.IsValid() && !IsSpellContainerNB(Damager) // No projectiles and no spell parry!
        && DamagerOwnerAction != gEAction_GetUpAttack // Parrying GetUpAttack of NPCs is easily exploitable :(
        && ScriptAdmin.CallScriptFromScript("CanParadeMoveOf", &Victim, &DamagerOwner, 0))
    {
        // Parry mechanic!
        if (Victim.Routine.GetCurrentState() == "NB_Melee_Parry")
        {
            if (Victim.NPC.GetCurrentMovementAni().Contains("Hit"))
            {
                if (!ScriptAdmin.CallScriptFromScript("IsInFistMode", &Victim, &None, 0))
                {
                    if (Damager.CollisionShape.GetPhysicMaterial() != eEShapeMaterial_Metal
                        || (Victim.Inventory.GetItemFromSlot(gESlot_RightHand).CollisionShape.GetPhysicMaterial()
                            != eEShapeMaterial_Metal))
                    {
                        EffectSystem::StartEffect("eff_col_weaponhitslevel_metal_wood_01", Victim);
                    }
                    else
                    {
                        EffectSystem::StartEffect("eff_col_wh_01_me_me", Victim);
                    }
                }

                if (!Damager.GetName().Contains("Fist"))
                {
                    DamagerOwner.Routine.FullStop();
                }

                DamagerOwner.Routine.SetTask("NB_ParryStumble");
                return gEAction_Parade;
            }
        }
    }

    Entity Player = Entity::GetPlayer();

    if (DamagerOwnerAction == gEAction_SprintAttack || DamagerOwnerAction == gEAction_PowerAttack)
    {
        if (Victim == Player)
        {
            Victim.NPC.SetLastHitTimestamp(0);
            DamagerOwner.Routine.AccessProperty<PSRoutine::PropertyAction>() = gEAction_PowerAttack;
        }
        else
        {
            DamagerOwnerAction = gEAction_PowerAttack;
        }
    }

    // Calc weapon damage (WAF-SCHD)
    const GEI32 iDamageAmount = Damager.Damage.GetProperty<PSDamage::PropertyDamageAmount>();
    const GEFloat fDamageMultiplier = Damager.Damage.GetProperty<PSDamage::PropertyDamageHitMultiplier>();
    const GEInt iWeaponDamage = static_cast<GEInt>(fDamageMultiplier * iDamageAmount);
    // const bCString VictimItemTemplateName =
    //    Victim.Inventory.GetTemplateItem(Victim.Inventory.FindStackIndex(gESlot_RightHand)).GetName();

    // Handle a few special cases if the player is the damager.
    if (Player == DamagerOwner)
    {
        // Player is behind (not in his FOV) an unsuspicious (following his daily routine) NPC.
        if (Victim.Routine.GetProperty<PSRoutine::PropertyAIMode>() == gEAIMode_Routine
            && !Victim.IsInFOV(DamagerOwner))
        {
            // PierceAttack is prerequisite for use of Perk_Assassin.
            if (DamagerOwnerAction == gEAction_PierceAttack)
            {
                if (Player.Inventory.IsSkillActive("Perk_Assassin"))
                {
                    // Victim loses all its HP
                    ScriptAdmin.CallScriptFromScript("SetHitPoints", &Victim, &None, 0);
                    Victim.Routine.FullStop();

                    if (ScriptAdmin.CallScriptFromScript("CanBeKilled", &Victim, &None, 0))
                    {
                        if (ScriptAdmin.CallScriptFromScript("IsHumanoid", &Victim, &None, 0))
                        {
                            Victim.Routine.SetTask("ZS_PiercedDead");
                            return gEAction_LiePiercedDead;
                        }
                        Victim.Routine.SetTask("ZS_RagDollDead");
                        return gEAction_LieDead;
                    }
                    else
                    {
                        Victim.Routine.SetTask("ZS_LieKnockDown");
                        return gEAction_LieKnockDown;
                    }
                }
            }
            // PowerAttack with a impact Weapon is prerequisite for Perk_KnockOut.
            else if ((DamagerOwnerAction == gEAction_PowerAttack || DamagerOwnerAction == gEAction_SprintAttack)
                     && (Damager.Damage.GetProperty<PSDamage::PropertyDamageType>() == gEDamageType_Impact))
            {
                if (Player.Inventory.IsSkillActive("Perk_KnockOut"))
                {
                    Victim.Routine.FullStop();
                    Victim.Routine.SetTask("ZS_LieKnockDown");
                    return gEAction_LieKnockDown;
                }
            }
        }
    }

    // HitForce Berechnung
    // - HitForce = max(GetActionWeaponLevel(DamagerOwner) - GetShieldLevelBonus(Victim), 1)

    gEHitForce HitForce = gEHitForce_Minimal;
    if (IsSpellContainerNB(Damager))
    {
        HitForce = gEHitForce_Heavy;
    }
    else if (IsNormalProjectileNB(Damager))
    {
        // Bow tension, for crossbows always 1.0
        HitForce = Damager.Damage.GetProperty<PSDamage::PropertyDamageHitMultiplier>() >= 0.6f ? gEHitForce_Heavy
                                                                                               : gEHitForce_Minimal;
    }
    else if (DamagerOwner != None)
    {
        GEInt ActionWeaponLevel = GetActionWeaponLevelNB(DamagerOwner, DamagerOwnerAction);
        HitForce = static_cast<gEHitForce>(ActionWeaponLevel - GetShieldLevelBonusNB(Victim));
        HitForce = static_cast<gEHitForce>(HitForce - getHyperArmorPoints(Victim, VictimAction));

        println("ActionWeaponLevel: %d\tShieldLevelBonus: %d\tgetHyperArmorPoints: %d", ActionWeaponLevel,
                GetShieldLevelBonusNB(Victim), getHyperArmorPoints(Victim, VictimAction));

        if (HitForce <= NBConfig::poiseThreshold)
        {
            HitForce = gEHitForce_Minimal;
        }
        else if (HitForce <= gEHitForce_Normal)
        {
            HitForce = gEHitForce_Normal;
        }
    }

    GEBool isHeadshot = GEFalse;

    // PSCollisionShape.GetType() -> gCCollisionShape_PS.GetTouchType()
    if (Damager.CollisionShape.GetType() == eEPropertySetType_Animation)
    {
        bCString BoneName = Damager.CollisionShape.GetTouchingBone();
        if (BoneName.Contains("_Head", 0)) // Kopfschuss
        {
            isHeadshot = GETrue;
        }
    }

    GEInt FinalDamage = iWeaponDamage;

    // Headshot? -> Double damage
    if (isHeadshot)
    {
        FinalDamage *= 2;
    }

    // Player attacks, while not being transformed
    if (Player == DamagerOwner && !Player.NPC.IsTransformed())
    {
        // PC-ATTR-BONUS
        GEInt iAttributeBonusDamage = 0;
        // setze STR und DEX 100 als default wert an und rechne ohne Bonusschaden am Anfang
        GEInt dexterity =
            ScriptAdmin.CallScriptFromScript("GetDexterity", &DamagerOwner, &None, 0) - NBConfig::startDEX;
        GEInt strength = ScriptAdmin.CallScriptFromScript("GetStrength", &DamagerOwner, &None, 0) - NBConfig::startSTR;
        GEInt intelligence = ScriptAdmin.CallScriptFromScript("GetIntelligence", &DamagerOwner, &None, 0);

        // Magic damage
        if (IsSpellContainerNB(Damager))
        {
            // Magic projectiles do double damage, !but are capped to 200.
            // TODO: Remove doubling damage for projectile (Rather Add new more potent Spell)
            if (IsMagicProjectileNB(Damager))
            {
                FinalDamage *= 2;
            }

            // PropertyManaUsed depends on cast phase
            if (Damager.Damage.GetProperty<PSDamage::PropertyManaUsed>())
            {
                // Bonus = ( Schaden * Altes Wissen / 100 )
                GEFloat fIntelligenceModifier = intelligence / 100.0f;
                iAttributeBonusDamage = static_cast<GEInt>(FinalDamage * fIntelligenceModifier);
            }
            else
            {
                iAttributeBonusDamage = FinalDamage * 2;
            }
        }
        // Ranged damage
        if (IsNormalProjectileNB(Damager) == GETrue)
        {
            // print ( "UseType Left : %d" ,
            // DamagerOwner.Inventory.GetItemFromSlot(gESlot_LeftHand).Interaction.UseType );
            if (NBConfig::useStrengthForCrossbows
                && DamagerOwner.Inventory.GetItemFromSlot(gESlot_LeftHand)
                           .Interaction.GetProperty<PSInteraction::PropertyUseType>()
                       == gEUseType_CrossBow)
            {
                iAttributeBonusDamage = strength / 2;
            }
            else
            {
                iAttributeBonusDamage = dexterity / 2;
            }
        }
        // Melee damage
        else
        {
            GEInt rightWeaponStackIndex = Player.Inventory.FindStackIndex(gESlot_RightHand);
            GEInt leftWeaponStackIndex = Player.Inventory.FindStackIndex(gESlot_LeftHand);
            gEUseType playerRightWeaponType = Player.Inventory.GetUseType(rightWeaponStackIndex);

            // New Scaling
            if (NBConfig::useNewBalanceMeleeScaling)
            {
                // TODO Use another Function for this bloaded if else statements (returns better)
                // GEChar* arr = nullptr;
                bCString reqAttributeTag = "";
                Entity Weapon = Player.GetWeapon(GETrue);
                if (Weapon != None && Weapon.Item.IsValid())
                {
                    // arr = ( GEChar* )*( DWORD* )( *( DWORD* )&Weapon.Item + 0x74 ); // A bit Unsafe ...
                    // AccessReqAttrib1Tag()
                    gCItem_PS *item = (gCItem_PS *)Weapon.Item.m_pEngineEntityPropertySet;
                    reqAttributeTag = item->AccessReqAttrib1Tag();
                }
                // if ( arr != nullptr ) reqAttributeTag = bCString ( arr );
                if (playerRightWeaponType == gEUseType_Staff || reqAttributeTag == "INT"
                    || (DamagerOwner.Inventory.GetItemFromSlot(gESlot_RightHand) != None
                        && DamagerOwner.Inventory.GetItemFromSlot(gESlot_RightHand).IsItem()
                        && DamagerOwner.Inventory.GetItemFromSlot(gESlot_RightHand).Item.GetQuality()
                               & (gEItemQuality_Burning | gEItemQuality_Frozen)))
                {
                    if ((playerRightWeaponType == gEUseType_1H
                         && Player.Inventory.GetUseType(leftWeaponStackIndex) == gEUseType_1H)
                        || reqAttributeTag == "DEX")
                    {
                        iAttributeBonusDamage =
                            static_cast<GEInt>(dexterity * 0.3 + intelligence * 0.35
                                               + 15); // Because you start with low Int, (Assume 60 INT)
                    }
                    else if (playerRightWeaponType == gEUseType_Staff)
                    {
                        iAttributeBonusDamage =
                            static_cast<GEInt>(strength * 0.2 + intelligence * 0.4
                                               + 15); // Because you start with low Int, (Assume 60 INT)
                    }
                    else if (playerRightWeaponType == gEUseType_2H || playerRightWeaponType == gEUseType::gEUseType_Axe
                             || playerRightWeaponType == gEUseType::gEUseType_Pickaxe)
                    {
                        iAttributeBonusDamage =
                            static_cast<GEInt>(strength * 0.3 + intelligence * 0.35
                                               + 15); // Because you start with low Int, (Assume 60 INT)
                    }
                    else
                    {
                        iAttributeBonusDamage =
                            static_cast<GEInt>(strength * 0.2 + intelligence * 0.4
                                               + 15); // Because you start with low Int, (Assume 60 INT)
                    }
                }
                else if (playerRightWeaponType == gEUseType_1H
                         && Player.Inventory.GetUseType(leftWeaponStackIndex) == gEUseType_1H)
                {
                    iAttributeBonusDamage = static_cast<GEInt>(strength * 0.3 + dexterity * 0.35);
                }
                else if (reqAttributeTag == "DEX")
                {
                    iAttributeBonusDamage = static_cast<GEInt>(strength * 0.2 + dexterity * 0.4);
                }
                else if (playerRightWeaponType == gEUseType_2H)
                {
                    iAttributeBonusDamage = static_cast<GEInt>(strength * 0.55);
                }
                else if (playerRightWeaponType == gEUseType::gEUseType_Axe
                         || playerRightWeaponType == gEUseType::gEUseType_Pickaxe)
                {
                    iAttributeBonusDamage = static_cast<GEInt>(strength * 0.6);
                }
                else
                {
                    iAttributeBonusDamage = strength / 2;
                }
            }
            else
            {
                iAttributeBonusDamage = strength / 2;
            }
        }
        if (iAttributeBonusDamage < 0)
        {
            iAttributeBonusDamage = 0;
        }
        FinalDamage += ScriptAdmin.CallScriptFromScript("GetQualityBonus", &Damager, &Victim, FinalDamage);
        FinalDamage += iAttributeBonusDamage;
        // Adjust GetPlayerSkillDamageBonus
        FinalDamage += ScriptAdmin.CallScriptFromScript("GetPlayerSkillDamageBonus", &Damager, &Victim, FinalDamage);
    }
    // Damager is transformed player or NPC
    else if (DamagerOwner.Navigation.IsValid())
    {
        GEInt iStrength = static_cast<GEInt>(ScriptAdmin.CallScriptFromScript("GetStrength", &DamagerOwner, &None, 0)
                                                 * NBConfig::NPCStrengthMultiplicator
                                             + NBConfig::NPCStrengthCorrection); // STR Bonus Real
        if (iStrength < 10)
            iStrength = 10;

        if (Damager.GetName() == "Fist" && ScriptAdmin.CallScriptFromScript("IsHumanoid", &DamagerOwner, &None, 0))
        {
            // Greift ein Mensch oder Ork mit Fäusten an?
            FinalDamage = static_cast<GEInt>(iStrength / 2);
        }
        else
        {
            if (IsNormalProjectileNB(Damager) || IsMagicProjectileNB(Damager) || IsSpellContainerNB(Damager))
            {
                FinalDamage = static_cast<GEInt>(iStrength + FinalDamage * NBConfig::npcWeaponDamageMultiplier);
            }
            // Greift ein Ork mit einer Nahkampfwaffe an?
            else if (DamagerOwner.NPC.GetProperty<PSNpc::PropertySpecies>() == gESpecies_Orc)
            {
                FinalDamage =
                    static_cast<GEInt>(iStrength
                                       + FinalDamage * NBConfig::npcWeaponDamageMultiplier
                                             / 2.0f); // TODO: Should 2H Weapon in 1H only do half weapon damage?
            }
            // Greift ein Mensch mit einer Nahkampfwaffe an?
            else if (DamagerOwner.NPC.GetProperty<PSNpc::PropertySpecies>() == gESpecies_Human)
            {
                FinalDamage = static_cast<GEInt>(iStrength + FinalDamage * NBConfig::npcWeaponDamageMultiplier);
            }
            else if (DamagerOwner.NPC.GetProperty<PSNpc::PropertySpecies>() == gESpecies_Demon
                     || DamagerOwner.NPC.GetProperty<PSNpc::PropertySpecies>() == gESpecies_Ogre)
            {
                FinalDamage =
                    static_cast<GEInt>(iStrength
                                       + FinalDamage * NBConfig::npcWeaponDamageMultiplier
                                             / 2.0f); // TODO: Should 2H Weapon in 1H only do half weapon damage?
            }
            // MonsterAttack
            else if (Damager.GetName() == "Fist")
            {
                FinalDamage = static_cast<GEInt>(
                    iStrength * 1.3f
                    + FinalDamage); // Change and simplify after Adding Unique "Fist"-Weapons for Monsters
            }
            else if (FinalDamage == 0)
            {
                FinalDamage = iStrength;
            }
            else
            {
                FinalDamage = static_cast<GEInt>(iStrength + FinalDamage * NBConfig::npcWeaponDamageMultiplier);
            }
        }

        // Magic Projectiles, casted by NPCs (against NPCs), do double damage (Modified)
        if (IsMagicProjectileNB(Damager) && !Victim.IsPlayer())
        {
            FinalDamage *= 2;
        }
    }

    if ((Damager.IsItem() && (Damager.Item.GetQuality() & gEItemQuality_Blessed) == gEItemQuality_Blessed
         && ScriptAdmin.CallScriptFromScript("IsEvil", &Victim, NULL, 0)))
        FinalDamage = static_cast<GEInt>(FinalDamage * 1.2f);

    if (!NBConfig::useNewStaminaMechanic)
    {
        if (GetHeldWeaponCategoryNB(DamagerOwner) == gEWeaponCategory_Melee)
        {
            if (GetScriptAdmin().CallScriptFromScript("GetStaminaPoints", &DamagerOwner, &None, 0) <= 50)
                FinalDamage = static_cast<GEInt>(FinalDamage * 0.7f);
            else if (GetScriptAdmin().CallScriptFromScript("GetStaminaPoints", &DamagerOwner, &None, 0) <= 20)
                FinalDamage = static_cast<GEInt>(FinalDamage * 0.5f);
        }
    }

    // Handelt es sich um einen Powercast? (Player and NPCs)
    if (Damager.Projectile.IsValid() && IsSpellContainerNB(Damager))
    {
        // Powercast
        if (Damager.Projectile.GetProperty<PSProjectile::PropertyPathStyle>() == gEProjectilePath_Missile)
        {
            FinalDamage *= 2;
        }
    }

    //
    // Schritt 2: Rüstung
    //
    GEInt FinalDamage2 = 0;
    GEInt iProtection = ScriptAdmin.CallScriptFromScript("GetProtection", &Victim, &Damager, 0);
    // println ( "Protection Integer %d" , iProtection );

    // Vulnerabilities
    switch (DamageTypeEntityTestNB(Victim, Damager))
    {
        case VulnerabilityStatus_WEAK:           FinalDamage = static_cast<GEInt>(FinalDamage * 1.6f); break;
        case VulnerabilityStatus_STRONG:         FinalDamage = static_cast<GEInt>(FinalDamage * 0.5f); break;
        case VulnerabilityStatus_SLIGHTLYWEAK:   FinalDamage = static_cast<GEInt>(FinalDamage * 1.2f); break;
        case VulnerabilityStatus_SLIGHTLYSTRONG: FinalDamage = static_cast<GEInt>(FinalDamage * 0.8f); break;
    }
    if (iProtection > 90)
        iProtection = 90;
    FinalDamage2 = FinalDamage - static_cast<GEInt>(FinalDamage * (iProtection / 100.0f));

    // print ( "Protection After Vulnerabilities %f\n ", fProtection );
    if (FinalDamage2 < 5)
        FinalDamage2 = 5;

    //
    // Schritt 3: Angriffsart
    //

    switch (DamagerOwner.Routine.GetProperty<PSRoutine::PropertyAction>())
    {
            // Angreifer benutzt Quick-Attacke
            //   => Schaden = Schaden / 2
        case gEAction_QuickAttack:
        case gEAction_QuickAttackR:
        case gEAction_QuickAttackL:
            // Quickattacken sind weniger effektiv gegen Starke NPC oder hohe Rüstung (5%)
            FinalDamage2 = static_cast<GEInt>(FinalDamage2 * (1.0f + NBConfig::QuickAttackArmorRes)
                                              - FinalDamage * NBConfig::QuickAttackArmorRes)
                         / 2;
            break;

            // Angreifer benutzt Powerattacke
            //   => Schaden = Schaden * 2 (bei Kampf mit 2 Waffen nicht durchgehend!)
        case gEAction_PowerAttack:
        case gEAction_SprintAttack:
            if (!CheckHandUseTypesNB(gEUseType_1H, gEUseType_1H, DamagerOwner)
                || DamagerOwner.Routine.GetProperty<PSRoutine::PropertyStatePosition>() == 2)
            {
                // Starke Attacken ignorieren 10 % Rüstung
                FinalDamage2 = static_cast<GEInt>(FinalDamage2 * (1.0f - NBConfig::PowerAttackArmorPen)
                                                  + FinalDamage * NBConfig::PowerAttackArmorPen)
                             * 2;
            }
            break;

            // Angreifer benutzt Hack-Attacke
            //   => Schaden = Schaden * 2
        case gEAction_HackAttack:
            // Hackattacken ignorieren 12.5 % R�stung
            FinalDamage2 = static_cast<GEInt>(FinalDamage2 * (1.0f - NBConfig::SpecialAttackArmorPen)
                                              + FinalDamage * NBConfig::SpecialAttackArmorPen)
                         * 2;
            break;
    }

    if (Victim.Routine.GetCurrentState() == "NB_ParryStumble")
    {
        FinalDamage2 = static_cast<GEInt>(FinalDamage2 * NBConfig::PerfectBlockDamageMult);
        if (static_cast<GEInt>(HitForce) >= 3)
        {
            HitForce = static_cast<gEHitForce>(NBConfig::KnockDownThreshold);
        }
        else
        {
            HitForce = gEHitForce_Normal;
        }
    }

    // Monster attacks Orc or Human (NPC)
    if (ScriptAdmin.CallScriptFromScript("IsHumanoid", &Victim, &None, 0)
        && (!ScriptAdmin.CallScriptFromScript("IsHumanoid", &DamagerOwner, &None, 0)) && (Player != DamagerOwner)
        && (Player != Victim))
    {
        if (DamagerOwner.Party.GetProperty<PSParty::PropertyPartyMemberType>() == gEPartyMemberType_Summoned)
        {
            // FinalDamage /= 2;
        }
        else
        {
            FinalDamage = static_cast<GEInt>(FinalDamage * NBConfig::fMonsterDamageMultiplicator);
        }
    }

    // New Multiplier for NPC vs NPC Damage
    if (!DamagerOwner.IsPlayer() && !Victim.IsPlayer())
    {
        FinalDamage2 = static_cast<GEInt>(FinalDamage2 * NBConfig::NPCDamageReductionMultiplicator);
    }

    if (FinalDamage2 < 5)
        FinalDamage2 = 5;

    if (DamageTypeEntityTestNB(Victim, Damager) == VulnerabilityStatus_IMMUNE && FinalDamage2 > 5)
        FinalDamage2 = 5;

    //
    // Schritt 4: Parade
    //
    /*
       Voraussetzungen für erfolgreiche Paraden:
         1. Das Opfer befindet sich im Parade-Modus.
         2. Der Angriff kommt von vorne.
         3. Bei Verteidigung mit Fäusten kann man nur parieren, wenn der Angreifer auch Fäuste benutzt (Boxkampf).
         4. Für Nahkampf-Angriffe gilt:
           - Hack-Attacken können nie pariert werden.
           - Sprint-Attacken können nie pariert werden. (Sprint-Attacken tauchen nur mit AB auf.)
           - Stich-Attacken kann man nur mit einem Schild abwehren.
         5. Magie-Angriffe kann nur der Held abwehren. Voraussetzung: Er beherrscht "Magische Stäbe" und hält in seinen
       Händen einen Stab und einen Zauberspruch.
         6. Fernkampf-Angriffe kann man nur mit einem Schild abwehren.
    */
    gCDamageReceiver_PS_Ext *pVictimDamageReceiver =
        GetPropertySet<gCDamageReceiver_PS_Ext>(Victim.GetGameEntity(), eEPropertySetType_DamageReceiver);

    if (Victim == Player)
        Victim.Effect.StopEffect(GETrue);
    // Parade Magic
    if (IsMagicProjectileNB(Damager))
    {
        if (!Victim.NPC.IsFrozen() && ScriptAdmin.CallScriptFromScript("CanParadeMagic", &Victim, &Damager, 0))
        {
            GEInt iManaPenalty = -FinalDamage;
            if (eCApplication::GetInstance().GetEngineSetup().AlternativeBalancing)
            {
                GEFloat fPenaltyModifier =
                    1.0f - ((ScriptAdmin.CallScriptFromScript("GetIntelligence", &Victim, &None, 0) - 200) / 500.0f);
                if (fPenaltyModifier > 0)
                {
                    if (fPenaltyModifier < 1)
                        iManaPenalty = static_cast<GEInt>(fPenaltyModifier * iManaPenalty);
                }
                else
                {
                    iManaPenalty = 0;
                }
            }
            GEInt iManaRemaining = iManaPenalty + ScriptAdmin.CallScriptFromScript("GetManaPoints", &Victim, &None, 0);
            if (iManaRemaining > 0)
                iManaRemaining = 0;

            ScriptAdmin.CallScriptFromScript("AddManaPoints", &Victim, &None, iManaPenalty);
            ScriptAdmin.CallScriptFromScript("AddHitPoints", &Victim, &None, iManaRemaining);
            if (ScriptAdmin.CallScriptFromScript("GetHitPoints", &Victim, &None, 0) > 0)
            {
                Victim.Routine.FullStop();
                Victim.Routine.SetTask("ZS_ParadeStumble");
                return gEAction_MagicParade;
            }
        }
    }
    // Parade Missile
    else if (IsNormalProjectileNB(Damager))
    {
        if (!Victim.NPC.IsFrozen() && ScriptAdmin.CallScriptFromScript("CanParadeMissile", &Victim, &Damager, 0))
        {
            GEInt iStaminaPenalty = -FinalDamage;
            if (Victim == Player && Victim.Inventory.IsSkillActive("Perk_Shield_2"))
                iStaminaPenalty /= 2;

            GEInt iStaminaRemaining =
                iStaminaPenalty + ScriptAdmin.CallScriptFromScript("GetStaminaPoints", &Victim, &None, 0);
            if (iStaminaRemaining > 0)
                iStaminaRemaining = 0;

            ScriptAdmin.CallScriptFromScript("AddStaminaPoints", &Victim, &None, iStaminaPenalty);
            ScriptAdmin.CallScriptFromScript("AddHitPoints", &Victim, &None, iStaminaRemaining);
            if (ScriptAdmin.CallScriptFromScript("GetHitPoints", &Victim, &None, 0) > 0)
            {
                if (HitForce > gEHitForce_Minimal)
                {
                    Victim.Routine.FullStop();
                    Victim.Routine.SetTask("ZS_ParadeStumble");
                    return gEAction_ParadeStumble;
                }
                return gEAction_QuickParadeStumble;
            }
        }
    }
    // Can parade meele?
    else if (!Victim.NPC.IsFrozen()
             && (ScriptAdmin.CallScriptFromScript("CanParade", &Victim, &DamagerOwner, 0)
                 // Special Perfect Block now!
                 || (NBConfig::enablePerfectBlock && (!NBConfig::playerOnlyPerfectBlock || Victim.IsPlayer())
                     && pVictimDamageReceiver && pVictimDamageReceiver->IsValid()
                     && pVictimDamageReceiver->GetPerfectBlockDelayed() == 0 && Victim.Routine.GetStateTime() < 0.1f
                     && Victim.Routine.GetProperty<PSRoutine::PropertyAniState>() == gEAniState_Parade
                     && Victim.IsInFOV(DamagerOwner))))
    {
        if (GetHeldWeaponCategoryNB(DamagerOwner) == gEWeaponCategory_Melee)
        {
            if (NBConfig::enablePerfectBlock && (!NBConfig::playerOnlyPerfectBlock || Victim.IsPlayer())
                && pVictimDamageReceiver && pVictimDamageReceiver->IsValid()
                && pVictimDamageReceiver->GetPerfectBlockDelayed() == 0
                && (Victim.Routine.GetStateTime() < 0.1f
                    || (DamagerOwnerAction != gEAction_PowerAttack && DamagerOwnerAction != gEAction_HackAttack
                        && DamagerOwnerAction != gEAction_SprintAttack && Victim.Routine.GetStateTime() < 0.15f)))
            {
                pVictimDamageReceiver->SetLastBlockTimeStamp(0);
                pVictimDamageReceiver->SetPerfectBlockDelayed(0);
                Victim.Routine.SetStateTime(0.0f); // Reset Timing

                if (!ScriptAdmin.CallScriptFromScript("IsInFistMode", &Victim, &None, 0))
                {
                    if (Damager.CollisionShape.GetPhysicMaterial() != eEShapeMaterial_Metal
                        || (Victim.Inventory.GetItemFromSlot(gESlot_RightHand).CollisionShape.GetPhysicMaterial()
                            != eEShapeMaterial_Metal))
                    {
                        EffectSystem::StartEffect("eff_col_weaponhitslevel_metal_wood_01", Victim);
                    }
                    else
                    {
                        EffectSystem::StartEffect("eff_col_wh_01_me_me", Victim);
                    }
                }

                gEAction stumbleAction = gEAction_StumbleL;
                if (Victim.Routine.GetProperty<PSRoutine::PropertyHitDirection>() == gEHitDirection_Right)
                    stumbleAction = gEAction_StumbleR;

                gCScriptProcessingUnit::sAICombatMoveInstr_Args InstrArgs(
                    DamagerOwner.GetInstance(), Victim.GetInstance(), stumbleAction, bCString("Hit"),
                    1.5f); // g_pstrPhaseString[gEPhase_Recover]

                gCScriptProcessingUnit::sAICombatMoveInstr(&InstrArgs, a_pSPU, GEFalse);
                return gEAction_ParadeStumble;
            }
        }

        GEFloat staminaDamageMultiplier = -0.5f;
        // Reduce damage if parading melee with shield
        if (Victim == Player && CheckHandUseTypesNB(gEUseType_Shield, gEUseType_1H, Victim))
        {
            if (!Victim.Inventory.IsSkillActive("Perk_Shield_2"))
            {
                // Weicht von "Detaillierte Schadenberechnung" ab, dort wird ein Faktor von 2/3 anstatt 0.5 beschrieben.
                staminaDamageMultiplier *= 0.6f;
            }
            else
            {
                staminaDamageMultiplier *= 0.4f;
            }
        }
        else
        {
            // NPC now always get less Stamina consumption (they really block often and the pc hero does much more
            // damage than an npc still) , right now they also don't use consumables, Hopefully soon :)
            staminaDamageMultiplier *= 0.5f; // 0.25
            if (CheckHandUseTypesNB(gEUseType_Shield, gEUseType_1H, Victim))
            {
                staminaDamageMultiplier *= 0.5f; // 0.125
            }
        }
        println("StaminaDamageMultiplier: %f", staminaDamageMultiplier);
        GEInt FinalDamage3 = static_cast<GEInt>(FinalDamage * staminaDamageMultiplier);
        println("FinalDamage3: %d", FinalDamage3);

        // AlternativeAI parade (es werden keine Lebenspunkte angezogen)
        if (NBConfig::useStaticBlocks && eCApplication::GetInstance().GetEngineSetup().AlternativeAI)
        {
            // Ausdauer abziehen
            ScriptAdmin.CallScriptFromScript("AddStaminaPoints", &Victim, &None, FinalDamage3);

            if (!ScriptAdmin.CallScriptFromScript("IsInFistMode", &Victim, &None, 0))
            {
                if (Damager.CollisionShape.GetPhysicMaterial() != eEShapeMaterial_Metal
                    || (Victim.Inventory.GetItemFromSlot(gESlot_RightHand).CollisionShape.GetPhysicMaterial()
                        != eEShapeMaterial_Metal))
                {
                    EffectSystem::StartEffect("eff_col_weaponhitslevel_metal_wood_01", Victim);
                }
                else
                {
                    EffectSystem::StartEffect("eff_col_wh_01_me_me", Victim);
                }
            }

            gCScriptProcessingUnit::sAICombatMoveInstr_Args InstrArgs(DamagerOwner.GetInstance(), Victim.GetInstance(),
                                                                      DamagerOwnerAction, bCString("Recover"),
                                                                      1.5f); // g_pstrPhaseString[gEPhase_Recover]
            if (!gCScriptProcessingUnit::sAICombatMoveInstr(&InstrArgs, a_pSPU, GEFalse))
                return gEAction_ParadeStumble;

            if (HitForce > gEHitForce_Normal)
            {
                if (HitForce > gEHitForce_Heavy)
                {
                    Victim.Routine.FullStop();
                    Victim.Routine.SetTask("ZS_HeavyParadeStumble");
                    return gEAction_HeavyParadeStumble;
                }
                else
                {
                    Victim.Routine.FullStop();
                    Victim.Routine.SetTask("ZS_ParadeStumble");
                    return gEAction_ParadeStumble;
                }
            }

            Victim.Routine.FullStop();
            Victim.Routine.SetTask("ZS_ParadeStumble");
            return gEAction_QuickParadeStumble;
        }

        // Ausdauer und ggf. Lebenspunkte abziehen
        GEInt iStaminaRemaining =
            FinalDamage3 + ScriptAdmin.CallScriptFromScript("GetStaminaPoints", &Victim, &None, 0);
        if (iStaminaRemaining > 0)
            iStaminaRemaining = 0;
        ScriptAdmin.CallScriptFromScript("AddStaminaPoints", &Victim, &None, FinalDamage3);
        // Changed back the remaining raw Damage after Def. Reduction and Stamina consumption
        GEInt healthDamage = iStaminaRemaining * 2;
        if (FinalDamage != 0)
            healthDamage = iStaminaRemaining * 2 * FinalDamage2 / FinalDamage;
        ScriptAdmin.CallScriptFromScript("AddHitPoints", &Victim, &None, healthDamage);

        // Wenn der bei der Parade erhaltene Schaden das Opfer nicht unter 0 HP bringt
        if (ScriptAdmin.CallScriptFromScript("GetHitPoints", &Victim, &None, 0) > 0)
        {
            if (HitForce > gEHitForce_Normal)
            {
                if (HitForce > gEHitForce_Heavy)
                {
                    Victim.Routine.FullStop();
                    Victim.Routine.SetTask("ZS_HeavyParadeStumble");
                    return gEAction_HeavyParadeStumble;
                }
                else
                {
                    Victim.Routine.FullStop();
                    Victim.Routine.SetTask("ZS_ParadeStumble");
                    return gEAction_ParadeStumble;
                }
            }
            else
            {
                Victim.Routine.FullStop();
                Victim.Routine.SetTask("ZS_ParadeStumble");
                return gEAction_QuickParadeStumble;
            }
        }
    }

    // Reduce Damage for unkillable NPCs (obsolete)
    GEInt iVictimHitPoints = ScriptAdmin.CallScriptFromScript("GetHitPoints", &Victim, &None, 0);
    if (Victim != Player && !ScriptAdmin.CallScriptFromScript("CanBeKilled", &Victim, &None, 0))
    {
        if (FinalDamage2 > 30)
        {
            FinalDamage2 = 30;
        }
        // TODO: Invincible NPCs cannot be Killed atleast
        // They receive still some damage through blocks before
        // Atleast the engine does not let them get killed
        if (FinalDamage2 >= iVictimHitPoints)
        {
            FinalDamage2 = 0;
        }
    }

    // Modify damage in some situations, if AB is activated
    if (eCApplication::GetInstance().GetEngineSetup().AlternativeBalancing)
    {
        // Wenn es sich um einen Arenakampf handelt
        //  => Schaden = Schaden / 2
        /*if ((Victim != Player) && (Victim.NPC.GetProperty<PSNpc::PropertyAttackReason>() == gEAttackReason_Arena))
        {
            FinalDamage2 /= 2;
        }*/

        // Wenn der Held bei Schwierigkeitsgrad "hoch" eine Fernkampfwaffe benutzt
        //  => Schaden = Schaden * 1,2
        if (Victim != Player && IsNormalProjectileNB(Damager) && Entity::GetCurrentDifficulty() == EDifficulty_Hard)
        {
            FinalDamage2 = static_cast<GEInt>(FinalDamage2 * 1.2f);
        }
    }

    if (IsNormalProjectileNB(Damager) && ScriptAdmin.CallScriptFromScript("IsHumanoid", &Victim, &None, 0))
    {
        // Projectile is a Headbutt
        if (Damager.Damage.GetProperty<PSDamage::PropertyDamageType>() == gEDamageType_Impact)
        {
            if (ScriptAdmin.CallScriptFromScript("CanBeKilled", &Victim, &None, 0)
                && (ScriptAdmin.CallScriptFromScript("GetHitPoints", &Victim, &None, 0) - FinalDamage2) <= 0)
            {
                if (FinalDamage2 > 0)
                {
                    Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageAmount>() = FinalDamage2;
                    Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageType>() =
                        Damager.Damage.GetProperty<PSDamage::PropertyDamageType>();
                }
                if (ScriptAdmin.CallScriptFromScript("GetAttitude", &Victim, &DamagerOwner, 0)
                        == gEAttitude::gEAttitude_Hostile
                    || ScriptAdmin.CallScriptFromScript("GetAttitude", &Victim, &DamagerOwner, 0)
                           == gEAttitude::gEAttitude_Panic)
                {
                    Victim.Routine.FullStop();
                    Victim.Routine.SetTask("ZS_RagDollDead");
                    return gEAction_LieDead;
                }

                Victim.Routine.FullStop();
                Victim.Routine.SetTask("ZS_Unconscious");
                return gEAction_LieKnockDown;
            }

            if ((Damager.Damage.GetProperty<PSDamage::PropertyDamageHitMultiplier>()) >= 0.7f)
            {
                if (FinalDamage2 > 0)
                {
                    Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageAmount>() = FinalDamage2;
                    Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageType>() =
                        Damager.Damage.GetProperty<PSDamage::PropertyDamageType>();
                }
                Victim.Routine.FullStop();
                Victim.Routine.SetTask("ZS_SitKnockDown");
                return gEAction_SitKnockDown;
            }
        }
    }

    // Receive Damage (later outside of this function)
    if (FinalDamage2 > 0)
    {
        Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageAmount>() = FinalDamage2;
        Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageType>() =
            Damager.Damage.GetProperty<PSDamage::PropertyDamageType>();
        if (pVictimDamageReceiver && pVictimDamageReceiver->IsValid())
        {
            pVictimDamageReceiver->SetLastHealthDamage(Entity::GetWorldEntity().Clock.GetTimeStampInSeconds());
        }
    }

    // Process StatusEffects and Animations:
    // CanBurn
    if (!Victim.NPC.IsFrozen() && ScriptAdmin.CallScriptFromScript("CanBurn", &Victim, &Damager, FinalDamage2))
    {
        Victim.NPC.EnableStatusEffects(gEStatusEffect_Burning, GETrue);
        Victim.Effect.StartRuntimeEffect("eff_magic_firespell_target_01");
    }
    // Dead, KnockDown or KnockedOut
    if ((iVictimHitPoints - FinalDamage2) <= 0)
    {
        if (ScriptAdmin.CallScriptFromScript("IsDeadlyDamage", &Victim, &Damager, 0))
        {
            Victim.Routine.FullStop();
            Victim.Routine.SetTask("ZS_RagDollDead");
            return gEAction_LieDead;
        }

        if (DamagerOwner.Routine.GetProperty<PSRoutine::PropertyAction>() == gEAction_PierceAttack
            && ScriptAdmin.CallScriptFromScript("IsHumanoid", &Victim, &None, 0))
        {
            Victim.Routine.FullStop();
            Victim.Routine.SetTask("ZS_PiercedKO");
            return gEAction_LiePiercedKO;
        }

        Victim.Routine.FullStop();
        if (HitForce >= 3)
        {
            Victim.Routine.SetTask("ZS_Unconscious");
            return gEAction_LieKnockOut;
        }
        Victim.Routine.SetTask("ZS_LieKnockDown");
        return gEAction_LieKnockDown;
    }

    if (ScriptAdmin.CallScriptFromScript("CanBePoisoned", &Victim, &Damager,
                                         DamagerOwnerAction == gEAction_PierceAttack
                                             || DamagerOwnerAction == gEAction_HackAttack))
    {
        if (pVictimDamageReceiver && pVictimDamageReceiver->IsValid())
        {
            pVictimDamageReceiver->AccessPoisonDamage() = GetPoisonDamage(DamagerOwner);
        }
        Victim.NPC.EnableStatusEffects(gEStatusEffect_Poisoned, GETrue);
    }

    if (ScriptAdmin.CallScriptFromScript("CanBeDiseased", &Victim, &Damager, 0))
    {
        Victim.NPC.EnableStatusEffects(gEStatusEffect_Diseased, GETrue);
    }

    if (ScriptAdmin.CallScriptFromScript("CanFreeze", &Victim, &Damager, FinalDamage2))
    {
        GEInt iFreezeTime = FinalDamage2 / 20;
        if (iFreezeTime < 5)
            iFreezeTime = 5;
        if (Damager.GetName() == "Mis_IceBlock")
            iFreezeTime = 30;
        Victim.Routine.FullStop();
        Victim.Routine.SetTask("ZS_Freeze");
        Victim.Routine.AccessProperty<PSRoutine::PropertyTaskPosition>() = 12 * iFreezeTime;
        return gEAction_None;
    }

    // Nostun Of animals when they are Mad
    if (!NBConfig::disableMonsterRage
        && Victim.Routine.GetProperty<PSRoutine::PropertyAction>() == gEAction_SprintAttack)
    {
        return gEAction_None;
    }

    // Stun Protection
    if (DamageTypeEntityTestNB(Victim, Damager) == VulnerabilityStatus_IMMUNE)
    {
        ScriptAdmin.CallScriptFromScript("AssessAgressor", &Victim, &DamagerOwner, 0);
        return gEAction_None;
    }

    if (VictimAction == gEAction_SitKnockDown)
    {
        ScriptAdmin.CallScriptFromScript("PipiStumble", &Victim, &None, 0);
        return gEAction_SitKnockDown;
    }

    if (DamagerOwner.Routine.GetProperty<PSRoutine::PropertyAction>() == gEAction_PierceAttack
        && ScriptAdmin.CallScriptFromScript("IsHumanoid", &Victim, &None, 0))
    {
        Victim.Routine.FullStop();
        Victim.Routine.SetTask("ZS_PierceStumble");
        return gEAction_PierceStumble;
    }
    // Freeze Reduced Timer on Hit

    // Magic Casting Protection!
    if (gEAction_Summon == Victim.Routine.GetProperty<PSRoutine::PropertyAction>()
        && 0.5f < Victim.Routine.GetStateTime() // Delay the protection a bit
        && !Victim.Interaction.GetSpell().GetName().Contains("Heal")
        && !Victim.Interaction.GetSpell().GetName().Contains("Cure"))
    {
        // If a special attack, or any strong attack-force is executed against the caster
        if (gEAction_HackAttack != DamagerOwnerAction && gEAction_PierceAttack != DamagerOwnerAction
            && NBConfig::KnockDownThreshold > HitForce && !isHeadshot)
        {
            ScriptAdmin.CallScriptFromScript("PipiStumble", &Victim, &None, 0); // Make Noice without Stumbles
            return gEAction_Stumble;
        }
    }

    // Troll are now more resistant to attacks but are a bit slower now
    if (Victim.NPC.GetProperty<PSNpc::PropertySpecies>() == gESpecies_Troll
        && (VictimAction == gEAction_PowerAttack || VictimAction == gEAction_SprintAttack
            || VictimAction == gEAction_Attack))
    {
        ScriptAdmin.CallScriptFromScript("PipiStumble", &Victim, &None, 0);
        return gEAction_QuickStumble;
    }

    // Scream or make HitEffect, but no Stumble also processes logic when you hit someone, like setting up combat mode
    if ((GEInt)HitForce <= gEHitForce_Minimal
        && (GetHeldWeaponCategoryNB(DamagerOwner) == gEWeaponCategory_Ranged || IsInActiveAttack(Victim)))
    {
        if (VictimAction == gEAction_PierceStumble)
        {
            Victim.Routine.FullStop();
            Victim.Routine.SetTask("ZS_QuickStumble");
            return gEAction_QuickStumble;
        }
        ScriptAdmin.CallScriptFromScript("PipiStumble", &Victim, &None, 0);
        return gEAction_QuickStumble;
    }

    if (GetHeldWeaponCategoryNB(Victim) != gEWeaponCategory_None
        && ScriptAdmin.CallScriptFromScript("IsHumanoid", &Victim, &None, 0))
    {
        if ((GEInt)HitForce >= NBConfig::
                KnockDownThreshold /* && GetHeldWeaponCategoryNB ( Victim ) == gEWeaponCategory_Melee */) // Remove
        {
            if (!NBConfig::onlyHeavyAttackKnockDown || DamagerOwnerAction == gEAction_HackAttack
                || DamagerOwnerAction == gEAction_PowerAttack || DamagerOwnerAction == gEAction_WhirlAttack)
            {
                Victim.Routine.FullStop();
                Victim.Routine.SetTask("ZS_SitKnockDown");
                return gEAction_SitKnockDown;
            }
        }

        if (HitForce < gEHitForce_Heavy)
        {
            Victim.Routine.FullStop();
            Victim.Routine.SetTask("ZS_QuickStumble");
            return gEAction_QuickStumble;
        }
    }

    Victim.Routine.FullStop();
    Victim.Routine.SetTask("ZS_Stumble");
    return gEAction_Stumble;
}

gEAction GE_STDCALL AssessHitNew(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                                 GEU32 a_iArgs)
{
    INIT_SCRIPT_EXT(Victim, Damager);
    UNREFERENCED_PARAMETER(a_iArgs);

    if (Victim == None || Damager == None)
        return gEAction_None;

    gCScriptAdmin &ScriptAdmin = GetScriptAdmin();
    // println("\n--------------------------------------------------------------\n");

    //
    // Schritt 1: Basisschaden
    //

    // Damager would be the weapon / projectile and DamagerOwner the attacking player / NPC.
    Entity DamagerOwner = Damager.Interaction.GetOwner();
    if (DamagerOwner == None && Damager.Navigation.IsValid())
    {
        DamagerOwner = Damager;
    }
    gEAction DamagerOwnerAction = DamagerOwner.Routine.GetProperty<PSRoutine::PropertyAction>();
    gEAction VictimAction = Victim.Routine.GetProperty<PSRoutine::PropertyAction>();

    Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageAmount>() = 0;
    Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageType>() = gEDamageType_Blade; // Default!
    Victim.NPC.SetLastAttacker(Victim.NPC.GetCurrentAttacker());
    Victim.NPC.SetCurrentAttacker(DamagerOwner);

    if (!Victim.NPC.IsFrozen() // Frozen Victim cannot parry!, should also never happen with state checks!
        && !Damager.Projectile.IsValid() && !IsSpellContainerNB(Damager) // No projectiles and no spell parry!
        && DamagerOwnerAction != gEAction_GetUpAttack // Parrying GetUpAttack of NPCs is easily exploitable :(
        && ScriptAdmin.CallScriptFromScript("CanParadeMoveOf", &Victim, &DamagerOwner, 0))
    {
        // Parry mechanic!
        if (Victim.Routine.GetCurrentState() == "NB_Melee_Parry")
        {
            if (Victim.NPC.GetCurrentMovementAni().Contains("Hit"))
            {
                if (!ScriptAdmin.CallScriptFromScript("IsInFistMode", &Victim, &None, 0))
                {
                    if (Damager.CollisionShape.GetPhysicMaterial() != eEShapeMaterial_Metal
                        || (Victim.Inventory.GetItemFromSlot(gESlot_RightHand).CollisionShape.GetPhysicMaterial()
                            != eEShapeMaterial_Metal))
                    {
                        EffectSystem::StartEffect("eff_col_weaponhitslevel_metal_wood_01", Victim);
                    }
                    else
                    {
                        EffectSystem::StartEffect("eff_col_wh_01_me_me", Victim);
                    }
                }

                if (!Damager.GetName().Contains("Fist"))
                {
                    DamagerOwner.Routine.FullStop();
                }

                DamagerOwner.Routine.SetTask("NB_ParryStumble");
                return gEAction_Parade;
            }
        }
    }

    Entity Player = Entity::GetPlayer();

    if (DamagerOwnerAction == gEAction_SprintAttack || DamagerOwnerAction == gEAction_PowerAttack)
    {
        if (Victim == Player)
        {
            Victim.NPC.SetLastHitTimestamp(0);
            DamagerOwner.Routine.AccessProperty<PSRoutine::PropertyAction>() = gEAction_PowerAttack;
        }
        else
        {
            DamagerOwnerAction = gEAction_PowerAttack;
        }
    }

    // Calc weapon damage (WAF-SCHD)
    const GEI32 iDamageAmount = Damager.Damage.GetProperty<PSDamage::PropertyDamageAmount>();
    const GEFloat fDamageMultiplier = Damager.Damage.GetProperty<PSDamage::PropertyDamageHitMultiplier>();
    const GEInt iWeaponDamage = static_cast<GEInt>(fDamageMultiplier * iDamageAmount);
    // const bCString VictimItemTemplateName =
    //    Victim.Inventory.GetTemplateItem(Victim.Inventory.FindStackIndex(gESlot_RightHand)).GetName();

    // Handle a few special cases if the player is the damager.
    if (Player == DamagerOwner)
    {
        // Player is behind (not in his FOV) an unsuspicious (following his daily routine) NPC.
        if (Victim.Routine.GetProperty<PSRoutine::PropertyAIMode>() == gEAIMode_Routine
            && !Victim.IsInFOV(DamagerOwner))
        {
            // PierceAttack is prerequisite for use of Perk_Assassin.
            if (DamagerOwnerAction == gEAction_PierceAttack)
            {
                if (Player.Inventory.IsSkillActive("Perk_Assassin"))
                {
                    // Victim loses all its HP
                    ScriptAdmin.CallScriptFromScript("SetHitPoints", &Victim, &None, 0);
                    Victim.Routine.FullStop();

                    if (ScriptAdmin.CallScriptFromScript("CanBeKilled", &Victim, &None, 0))
                    {
                        if (ScriptAdmin.CallScriptFromScript("IsHumanoid", &Victim, &None, 0))
                        {
                            Victim.Routine.SetTask("ZS_PiercedDead");
                            return gEAction_LiePiercedDead;
                        }
                        Victim.Routine.SetTask("ZS_RagDollDead");
                        return gEAction_LieDead;
                    }
                    else
                    {
                        Victim.Routine.SetTask("ZS_LieKnockDown");
                        return gEAction_LieKnockDown;
                    }
                }
            }
            // PowerAttack with a impact Weapon is prerequisite for Perk_KnockOut.
            else if ((DamagerOwnerAction == gEAction_PowerAttack || DamagerOwnerAction == gEAction_SprintAttack)
                     && (Damager.Damage.GetProperty<PSDamage::PropertyDamageType>() == gEDamageType_Impact))
            {
                if (Player.Inventory.IsSkillActive("Perk_KnockOut"))
                {
                    Victim.Routine.FullStop();
                    Victim.Routine.SetTask("ZS_LieKnockDown");
                    return gEAction_LieKnockDown;
                }
            }
        }
    }

    // HitForce Berechnung
    // - HitForce = max(GetActionWeaponLevel(DamagerOwner) - GetShieldLevelBonus(Victim), 1)

    gEHitForce HitForce = gEHitForce_Minimal;
    if (IsSpellContainerNB(Damager))
    {
        HitForce = gEHitForce_Heavy;
    }
    else if (IsNormalProjectileNB(Damager))
    {
        // Bow tension, for crossbows always 1.0
        HitForce = Damager.Damage.GetProperty<PSDamage::PropertyDamageHitMultiplier>() >= 0.6f ? gEHitForce_Heavy
                                                                                               : gEHitForce_Minimal;
    }
    else if (DamagerOwner != None)
    {
        GEInt ActionWeaponLevel = GetActionWeaponLevelNB(DamagerOwner, DamagerOwnerAction);
        HitForce = static_cast<gEHitForce>(ActionWeaponLevel - GetShieldLevelBonusNB(Victim));
        HitForce = static_cast<gEHitForce>(HitForce - getHyperArmorPoints(Victim, VictimAction));

        println("ActionWeaponLevel: %d\tShieldLevelBonus: %d\tgetHyperArmorPoints: %d", ActionWeaponLevel,
                GetShieldLevelBonusNB(Victim), getHyperArmorPoints(Victim, VictimAction));

        if (HitForce <= NBConfig::poiseThreshold)
        {
            HitForce = gEHitForce_Minimal;
        }
        else if (HitForce <= gEHitForce_Normal)
        {
            HitForce = gEHitForce_Normal;
        }
    }

    GEBool isHeadshot = GEFalse;

    // PSCollisionShape.GetType() -> gCCollisionShape_PS.GetTouchType()
    if (Damager.CollisionShape.GetType() == eEPropertySetType_Animation)
    {
        bCString BoneName = Damager.CollisionShape.GetTouchingBone();
        if (BoneName.Contains("_Head", 0)) // Kopfschuss
        {
            isHeadshot = GETrue;
        }
    }

    GEInt FinalDamage = iWeaponDamage;

    // Headshot? -> Double damage
    if (isHeadshot)
    {
        FinalDamage *= 2;
    }

    // Player attacks, while not being transformed
    if (Player == DamagerOwner && !Player.NPC.IsTransformed())
    {
        // PC-ATTR-BONUS
        GEInt iAttributeBonusDamage = 0;
        // setze STR und DEX 100 als default wert an und rechne ohne Bonusschaden am Anfang
        GEInt dexterity =
            ScriptAdmin.CallScriptFromScript("GetDexterity", &DamagerOwner, &None, 0) - NBConfig::startDEX;
        GEInt strength = ScriptAdmin.CallScriptFromScript("GetStrength", &DamagerOwner, &None, 0) - NBConfig::startSTR;
        GEInt intelligence = ScriptAdmin.CallScriptFromScript("GetIntelligence", &DamagerOwner, &None, 0);

        // Magic damage
        if (IsSpellContainerNB(Damager))
        {
            // Magic projectiles do double damage, !but are capped to 200.
            // TODO: Remove doubling damage for projectile (Rather Add new more potent Spell)
            if (IsMagicProjectileNB(Damager))
            {
                FinalDamage *= 2;
            }

            // PropertyManaUsed depends on cast phase
            if (Damager.Damage.GetProperty<PSDamage::PropertyManaUsed>())
            {
                // Bonus = ( Schaden * Altes Wissen / 100 )
                GEFloat fIntelligenceModifier = intelligence / 100.0f;
                iAttributeBonusDamage = static_cast<GEInt>(FinalDamage * fIntelligenceModifier);
            }
            else
            {
                iAttributeBonusDamage = FinalDamage * 2;
            }
        }
        // Ranged damage
        if (IsNormalProjectileNB(Damager) == GETrue)
        {
            // print ( "UseType Left : %d" ,
            // DamagerOwner.Inventory.GetItemFromSlot(gESlot_LeftHand).Interaction.UseType );
            if (NBConfig::useStrengthForCrossbows
                && DamagerOwner.Inventory.GetItemFromSlot(gESlot_LeftHand)
                           .Interaction.GetProperty<PSInteraction::PropertyUseType>()
                       == gEUseType_CrossBow)
            {
                iAttributeBonusDamage = strength / 2;
            }
            else
            {
                iAttributeBonusDamage = dexterity / 2;
            }
        }
        // Melee damage
        else
        {
            GEInt rightWeaponStackIndex = Player.Inventory.FindStackIndex(gESlot_RightHand);
            GEInt leftWeaponStackIndex = Player.Inventory.FindStackIndex(gESlot_LeftHand);
            gEUseType playerRightWeaponType = Player.Inventory.GetUseType(rightWeaponStackIndex);

            // New Scaling
            if (NBConfig::useNewBalanceMeleeScaling)
            {
                // TODO Use another Function for this bloaded if else statements (returns better)
                // GEChar* arr = nullptr;
                bCString reqAttributeTag = "";
                Entity Weapon = Player.GetWeapon(GETrue);
                if (Weapon != None && Weapon.Item.IsValid())
                {
                    // arr = ( GEChar* )*( DWORD* )( *( DWORD* )&Weapon.Item + 0x74 ); // A bit Unsafe ...
                    // AccessReqAttrib1Tag()
                    gCItem_PS *item = (gCItem_PS *)Weapon.Item.m_pEngineEntityPropertySet;
                    reqAttributeTag = item->AccessReqAttrib1Tag();
                }
                // if ( arr != nullptr ) reqAttributeTag = bCString ( arr );
                if (playerRightWeaponType == gEUseType_Staff || reqAttributeTag == "INT"
                    || (DamagerOwner.Inventory.GetItemFromSlot(gESlot_RightHand) != None
                        && DamagerOwner.Inventory.GetItemFromSlot(gESlot_RightHand).IsItem()
                        && DamagerOwner.Inventory.GetItemFromSlot(gESlot_RightHand).Item.GetQuality()
                               & (gEItemQuality_Burning | gEItemQuality_Frozen)))
                {
                    if ((playerRightWeaponType == gEUseType_1H
                         && Player.Inventory.GetUseType(leftWeaponStackIndex) == gEUseType_1H)
                        || reqAttributeTag == "DEX")
                    {
                        iAttributeBonusDamage =
                            static_cast<GEInt>(dexterity * 0.3 + intelligence * 0.35
                                               + 15); // Because you start with low Int, (Assume 60 INT)
                    }
                    else if (playerRightWeaponType == gEUseType_Staff)
                    {
                        iAttributeBonusDamage =
                            static_cast<GEInt>(strength * 0.2 + intelligence * 0.4
                                               + 15); // Because you start with low Int, (Assume 60 INT)
                    }
                    else if (playerRightWeaponType == gEUseType_2H || playerRightWeaponType == gEUseType::gEUseType_Axe
                             || playerRightWeaponType == gEUseType::gEUseType_Pickaxe)
                    {
                        iAttributeBonusDamage =
                            static_cast<GEInt>(strength * 0.3 + intelligence * 0.35
                                               + 15); // Because you start with low Int, (Assume 60 INT)
                    }
                    else
                    {
                        iAttributeBonusDamage =
                            static_cast<GEInt>(strength * 0.2 + intelligence * 0.4
                                               + 15); // Because you start with low Int, (Assume 60 INT)
                    }
                }
                else if (playerRightWeaponType == gEUseType_1H
                         && Player.Inventory.GetUseType(leftWeaponStackIndex) == gEUseType_1H)
                {
                    iAttributeBonusDamage = static_cast<GEInt>(strength * 0.3 + dexterity * 0.35);
                }
                else if (reqAttributeTag == "DEX")
                {
                    iAttributeBonusDamage = static_cast<GEInt>(strength * 0.2 + dexterity * 0.4);
                }
                else if (playerRightWeaponType == gEUseType_2H)
                {
                    iAttributeBonusDamage = static_cast<GEInt>(strength * 0.55);
                }
                else if (playerRightWeaponType == gEUseType::gEUseType_Axe
                         || playerRightWeaponType == gEUseType::gEUseType_Pickaxe)
                {
                    iAttributeBonusDamage = static_cast<GEInt>(strength * 0.6);
                }
                else
                {
                    iAttributeBonusDamage = strength / 2;
                }
            }
            else
            {
                iAttributeBonusDamage = strength / 2;
            }
        }
        if (iAttributeBonusDamage < 0)
        {
            iAttributeBonusDamage = 0;
        }
        FinalDamage += ScriptAdmin.CallScriptFromScript("GetQualityBonus", &Damager, &Victim, FinalDamage);
        FinalDamage += iAttributeBonusDamage;
        // Adjust GetPlayerSkillDamageBonus
        FinalDamage += ScriptAdmin.CallScriptFromScript("GetPlayerSkillDamageBonus", &Damager, &Victim, FinalDamage);
    }
    // Damager is transformed player or NPC
    else if (DamagerOwner.Navigation.IsValid())
    {
        GEInt iStrength = static_cast<GEInt>(ScriptAdmin.CallScriptFromScript("GetStrength", &DamagerOwner, &None, 0)
                                                 * NBConfig::NPCStrengthMultiplicator
                                             + NBConfig::NPCStrengthCorrection); // STR Bonus Real
        if (iStrength < 10)
            iStrength = 10;

        if (Damager.GetName() == "Fist" && ScriptAdmin.CallScriptFromScript("IsHumanoid", &DamagerOwner, &None, 0))
        {
            // Greift ein Mensch oder Ork mit Fäusten an?
            FinalDamage = static_cast<GEInt>(iStrength / 2);
        }
        else
        {
            if (IsNormalProjectileNB(Damager) || IsMagicProjectileNB(Damager) || IsSpellContainerNB(Damager))
            {
                FinalDamage = static_cast<GEInt>(iStrength + FinalDamage * NBConfig::npcWeaponDamageMultiplier);
            }
            // Greift ein Ork mit einer Nahkampfwaffe an?
            else if (DamagerOwner.NPC.GetProperty<PSNpc::PropertySpecies>() == gESpecies_Orc)
            {
                FinalDamage =
                    static_cast<GEInt>(iStrength
                                       + FinalDamage * NBConfig::npcWeaponDamageMultiplier
                                             / 2.0f); // TODO: Should 2H Weapon in 1H only do half weapon damage?
            }
            // Greift ein Mensch mit einer Nahkampfwaffe an?
            else if (DamagerOwner.NPC.GetProperty<PSNpc::PropertySpecies>() == gESpecies_Human)
            {
                FinalDamage = static_cast<GEInt>(iStrength + FinalDamage * NBConfig::npcWeaponDamageMultiplier);
            }
            else if (DamagerOwner.NPC.GetProperty<PSNpc::PropertySpecies>() == gESpecies_Demon
                     || DamagerOwner.NPC.GetProperty<PSNpc::PropertySpecies>() == gESpecies_Ogre)
            {
                FinalDamage =
                    static_cast<GEInt>(iStrength
                                       + FinalDamage * NBConfig::npcWeaponDamageMultiplier
                                             / 2.0f); // TODO: Should 2H Weapon in 1H only do half weapon damage?
            }
            // MonsterAttack
            else if (Damager.GetName() == "Fist")
            {
                FinalDamage = static_cast<GEInt>(
                    iStrength * 1.3f
                    + FinalDamage); // Change and simplify after Adding Unique "Fist"-Weapons for Monsters
            }
            else if (FinalDamage == 0)
            {
                FinalDamage = iStrength;
            }
            else
            {
                FinalDamage = static_cast<GEInt>(iStrength + FinalDamage * NBConfig::npcWeaponDamageMultiplier);
            }
        }

        // Magic Projectiles, casted by NPCs (against NPCs), do double damage (Modified)
        if (IsMagicProjectileNB(Damager) && !Victim.IsPlayer())
        {
            FinalDamage *= 2;
        }
    }

    if ((Damager.IsItem() && (Damager.Item.GetQuality() & gEItemQuality_Blessed) == gEItemQuality_Blessed
         && ScriptAdmin.CallScriptFromScript("IsEvil", &Victim, NULL, 0)))
        FinalDamage = static_cast<GEInt>(FinalDamage * 1.2f);

    if (!NBConfig::useNewStaminaMechanic)
    {
        if (GetHeldWeaponCategoryNB(DamagerOwner) == gEWeaponCategory_Melee)
        {
            if (GetScriptAdmin().CallScriptFromScript("GetStaminaPoints", &DamagerOwner, &None, 0) <= 50)
                FinalDamage = static_cast<GEInt>(FinalDamage * 0.7f);
            else if (GetScriptAdmin().CallScriptFromScript("GetStaminaPoints", &DamagerOwner, &None, 0) <= 20)
                FinalDamage = static_cast<GEInt>(FinalDamage * 0.5f);
        }
    }

    // Handelt es sich um einen Powercast? (Player and NPCs)
    if (Damager.Projectile.IsValid() && IsSpellContainerNB(Damager))
    {
        // Powercast
        if (Damager.Projectile.GetProperty<PSProjectile::PropertyPathStyle>() == gEProjectilePath_Missile)
        {
            FinalDamage *= 2;
        }
    }

    //
    // Schritt 2: Rüstung
    //
    GEInt FinalDamage2 = 0;
    GEInt iProtection = ScriptAdmin.CallScriptFromScript("GetProtection", &Victim, &Damager, 0);
    // println ( "Protection Integer %d" , iProtection );

    // Vulnerabilities
    switch (DamageTypeEntityTestNB(Victim, Damager))
    {
        case VulnerabilityStatus_WEAK:           FinalDamage = static_cast<GEInt>(FinalDamage * 1.6f); break;
        case VulnerabilityStatus_STRONG:         FinalDamage = static_cast<GEInt>(FinalDamage * 0.5f); break;
        case VulnerabilityStatus_SLIGHTLYWEAK:   FinalDamage = static_cast<GEInt>(FinalDamage * 1.2f); break;
        case VulnerabilityStatus_SLIGHTLYSTRONG: FinalDamage = static_cast<GEInt>(FinalDamage * 0.8f); break;
    }
    if (iProtection > 90)
        iProtection = 90;
    FinalDamage2 = FinalDamage - static_cast<GEInt>(FinalDamage * (iProtection / 100.0f));

    // print ( "Protection After Vulnerabilities %f\n ", fProtection );
    if (FinalDamage2 < 5)
        FinalDamage2 = 5;

    //
    // Schritt 3: Angriffsart
    //

    switch (DamagerOwner.Routine.GetProperty<PSRoutine::PropertyAction>())
    {
        // Angreifer benutzt Quick-Attacke
        //   => Schaden = Schaden / 2
        case gEAction_QuickAttack:
        case gEAction_QuickAttackR:
        case gEAction_QuickAttackL:
            // Quickattacken sind weniger effektiv gegen Starke NPC oder hohe Rüstung (5%)
            FinalDamage2 = static_cast<GEInt>(FinalDamage2 * (1.0f + NBConfig::QuickAttackArmorRes)
                                              - FinalDamage * NBConfig::QuickAttackArmorRes)
                         / 2;
            break;

        // Angreifer benutzt Powerattacke
        //   => Schaden = Schaden * 2 (bei Kampf mit 2 Waffen nicht durchgehend!)
        case gEAction_PowerAttack:
        case gEAction_SprintAttack:
            if (!CheckHandUseTypesNB(gEUseType_1H, gEUseType_1H, DamagerOwner)
                || DamagerOwner.Routine.GetProperty<PSRoutine::PropertyStatePosition>() == 2)
            {
                // Starke Attacken ignorieren 10 % Rüstung
                FinalDamage2 = static_cast<GEInt>(FinalDamage2 * (1.0f - NBConfig::PowerAttackArmorPen)
                                                  + FinalDamage * NBConfig::PowerAttackArmorPen)
                             * 2;
            }
            break;

        // Angreifer benutzt Hack-Attacke
        //   => Schaden = Schaden * 2
        case gEAction_HackAttack:
            // Hackattacken ignorieren 12.5 % R�stung
            FinalDamage2 = static_cast<GEInt>(FinalDamage2 * (1.0f - NBConfig::SpecialAttackArmorPen)
                                              + FinalDamage * NBConfig::SpecialAttackArmorPen)
                         * 2;
            break;
    }

    if (Victim.Routine.GetCurrentState() == "NB_ParryStumble")
    {
        FinalDamage2 = static_cast<GEInt>(FinalDamage2 * NBConfig::PerfectBlockDamageMult);
        if (static_cast<GEInt>(HitForce) >= 3)
        {
            HitForce = static_cast<gEHitForce>(NBConfig::KnockDownThreshold);
        }
        else
        {
            HitForce = gEHitForce_Normal;
        }
    }

    // Monster attacks Orc or Human (NPC)
    if (ScriptAdmin.CallScriptFromScript("IsHumanoid", &Victim, &None, 0)
        && (!ScriptAdmin.CallScriptFromScript("IsHumanoid", &DamagerOwner, &None, 0)) && (Player != DamagerOwner)
        && (Player != Victim))
    {
        if (DamagerOwner.Party.GetProperty<PSParty::PropertyPartyMemberType>() == gEPartyMemberType_Summoned)
        {
            // FinalDamage /= 2;
        }
        else
        {
            FinalDamage = static_cast<GEInt>(FinalDamage * NBConfig::fMonsterDamageMultiplicator);
        }
    }

    // New Multiplier for NPC vs NPC Damage
    if (!DamagerOwner.IsPlayer() && !Victim.IsPlayer())
    {
        FinalDamage2 = static_cast<GEInt>(FinalDamage2 * NBConfig::NPCDamageReductionMultiplicator);
    }

    if (FinalDamage2 < 5)
        FinalDamage2 = 5;

    if (DamageTypeEntityTestNB(Victim, Damager) == VulnerabilityStatus_IMMUNE && FinalDamage2 > 5)
        FinalDamage2 = 5;

    //
    // Schritt 4: Parade
    //
    /*
    Voraussetzungen für erfolgreiche Paraden:
    1. Das Opfer befindet sich im Parade-Modus.
    2. Der Angriff kommt von vorne.
    3. Bei Verteidigung mit Fäusten kann man nur parieren, wenn der Angreifer auch Fäuste benutzt (Boxkampf).
    4. Für Nahkampf-Angriffe gilt:
    - Hack-Attacken können nie pariert werden.
    - Sprint-Attacken können nie pariert werden. (Sprint-Attacken tauchen nur mit AB auf.)
    - Stich-Attacken kann man nur mit einem Schild abwehren.
    5. Magie-Angriffe kann nur der Held abwehren. Voraussetzung: Er beherrscht "Magische Stäbe" und hält in seinen
    Händen einen Stab und einen Zauberspruch.
    6. Fernkampf-Angriffe kann man nur mit einem Schild abwehren.
    */
    gCDamageReceiver_PS_Ext *pVictimDamageReceiver =
        GetPropertySet<gCDamageReceiver_PS_Ext>(Victim.GetGameEntity(), eEPropertySetType_DamageReceiver);

    if (Victim == Player)
        Victim.Effect.StopEffect(GETrue);
    // Parade Magic
    if (IsMagicProjectileNB(Damager))
    {
        if (!Victim.NPC.IsFrozen() && ScriptAdmin.CallScriptFromScript("CanParadeMagic", &Victim, &Damager, 0))
        {
            GEInt iManaPenalty = -FinalDamage;
            if (eCApplication::GetInstance().GetEngineSetup().AlternativeBalancing)
            {
                GEFloat fPenaltyModifier =
                    1.0f - ((ScriptAdmin.CallScriptFromScript("GetIntelligence", &Victim, &None, 0) - 200) / 500.0f);
                if (fPenaltyModifier > 0)
                {
                    if (fPenaltyModifier < 1)
                        iManaPenalty = static_cast<GEInt>(fPenaltyModifier * iManaPenalty);
                }
                else
                {
                    iManaPenalty = 0;
                }
            }
            GEInt iManaRemaining = iManaPenalty + ScriptAdmin.CallScriptFromScript("GetManaPoints", &Victim, &None, 0);
            if (iManaRemaining > 0)
                iManaRemaining = 0;

            ScriptAdmin.CallScriptFromScript("AddManaPoints", &Victim, &None, iManaPenalty);
            ScriptAdmin.CallScriptFromScript("AddHitPoints", &Victim, &None, iManaRemaining);
            if (ScriptAdmin.CallScriptFromScript("GetHitPoints", &Victim, &None, 0) > 0)
            {
                Victim.Routine.FullStop();
                Victim.Routine.SetTask("ZS_ParadeStumble");
                return gEAction_MagicParade;
            }
        }
    }
    // Parade Missile
    else if (IsNormalProjectileNB(Damager))
    {
        if (!Victim.NPC.IsFrozen() && ScriptAdmin.CallScriptFromScript("CanParadeMissile", &Victim, &Damager, 0))
        {
            GEInt iStaminaPenalty = -FinalDamage;
            if (Victim == Player && Victim.Inventory.IsSkillActive("Perk_Shield_2"))
                iStaminaPenalty /= 2;

            GEInt iStaminaRemaining =
                iStaminaPenalty + ScriptAdmin.CallScriptFromScript("GetStaminaPoints", &Victim, &None, 0);
            if (iStaminaRemaining > 0)
                iStaminaRemaining = 0;

            ScriptAdmin.CallScriptFromScript("AddStaminaPoints", &Victim, &None, iStaminaPenalty);
            ScriptAdmin.CallScriptFromScript("AddHitPoints", &Victim, &None, iStaminaRemaining);
            if (ScriptAdmin.CallScriptFromScript("GetHitPoints", &Victim, &None, 0) > 0)
            {
                if (HitForce > gEHitForce_Minimal)
                {
                    Victim.Routine.FullStop();
                    Victim.Routine.SetTask("ZS_ParadeStumble");
                    return gEAction_ParadeStumble;
                }
                return gEAction_QuickParadeStumble;
            }
        }
    }
    // Can parade meele?
    else if (!Victim.NPC.IsFrozen()
             && (ScriptAdmin.CallScriptFromScript("CanParade", &Victim, &DamagerOwner, 0)
                 // Special Perfect Block now!
                 || (NBConfig::enablePerfectBlock && (!NBConfig::playerOnlyPerfectBlock || Victim.IsPlayer())
                     && pVictimDamageReceiver && pVictimDamageReceiver->IsValid()
                     && pVictimDamageReceiver->GetPerfectBlockDelayed() == 0 && Victim.Routine.GetStateTime() < 0.1f
                     && Victim.Routine.GetProperty<PSRoutine::PropertyAniState>() == gEAniState_Parade
                     && Victim.IsInFOV(DamagerOwner))))
    {
        if (GetHeldWeaponCategoryNB(DamagerOwner) == gEWeaponCategory_Melee)
        {
            if (NBConfig::enablePerfectBlock && (!NBConfig::playerOnlyPerfectBlock || Victim.IsPlayer())
                && pVictimDamageReceiver && pVictimDamageReceiver->IsValid()
                && pVictimDamageReceiver->GetPerfectBlockDelayed() == 0
                && (Victim.Routine.GetStateTime() < 0.1f
                    || (DamagerOwnerAction != gEAction_PowerAttack && DamagerOwnerAction != gEAction_HackAttack
                        && DamagerOwnerAction != gEAction_SprintAttack && Victim.Routine.GetStateTime() < 0.15f)))
            {
                pVictimDamageReceiver->SetLastBlockTimeStamp(0);
                pVictimDamageReceiver->SetPerfectBlockDelayed(0);
                Victim.Routine.SetStateTime(0.0f); // Reset Timing

                if (!ScriptAdmin.CallScriptFromScript("IsInFistMode", &Victim, &None, 0))
                {
                    if (Damager.CollisionShape.GetPhysicMaterial() != eEShapeMaterial_Metal
                        || (Victim.Inventory.GetItemFromSlot(gESlot_RightHand).CollisionShape.GetPhysicMaterial()
                            != eEShapeMaterial_Metal))
                    {
                        EffectSystem::StartEffect("eff_col_weaponhitslevel_metal_wood_01", Victim);
                    }
                    else
                    {
                        EffectSystem::StartEffect("eff_col_wh_01_me_me", Victim);
                    }
                }

                gEAction stumbleAction = gEAction_StumbleL;
                if (Victim.Routine.GetProperty<PSRoutine::PropertyHitDirection>() == gEHitDirection_Right)
                    stumbleAction = gEAction_StumbleR;

                gCScriptProcessingUnit::sAICombatMoveInstr_Args InstrArgs(
                    DamagerOwner.GetInstance(), Victim.GetInstance(), stumbleAction, bCString("Hit"),
                    1.5f); // g_pstrPhaseString[gEPhase_Recover]

                gCScriptProcessingUnit::sAICombatMoveInstr(&InstrArgs, a_pSPU, GEFalse);
                return gEAction_ParadeStumble;
            }
        }

        GEFloat staminaDamageMultiplier = -0.5f;
        // Reduce damage if parading melee with shield
        if (Victim == Player && CheckHandUseTypesNB(gEUseType_Shield, gEUseType_1H, Victim))
        {
            if (!Victim.Inventory.IsSkillActive("Perk_Shield_2"))
            {
                // Weicht von "Detaillierte Schadenberechnung" ab, dort wird ein Faktor von 2/3 anstatt 0.5 beschrieben.
                staminaDamageMultiplier *= 0.6f;
            }
            else
            {
                staminaDamageMultiplier *= 0.4f;
            }
        }
        else
        {
            // NPC now always get less Stamina consumption (they really block often and the pc hero does much more
            // damage than an npc still) , right now they also don't use consumables, Hopefully soon :)
            staminaDamageMultiplier *= 0.5f; // 0.25
            if (CheckHandUseTypesNB(gEUseType_Shield, gEUseType_1H, Victim))
            {
                staminaDamageMultiplier *= 0.5f; // 0.125
            }
        }
        println("StaminaDamageMultiplier: %f", staminaDamageMultiplier);
        GEInt FinalDamage3 = static_cast<GEInt>(FinalDamage * staminaDamageMultiplier);
        println("FinalDamage3: %d", FinalDamage3);

        // AlternativeAI parade (es werden keine Lebenspunkte angezogen)
        if (NBConfig::useStaticBlocks && eCApplication::GetInstance().GetEngineSetup().AlternativeAI)
        {
            // Ausdauer abziehen
            ScriptAdmin.CallScriptFromScript("AddStaminaPoints", &Victim, &None, FinalDamage3);

            if (!ScriptAdmin.CallScriptFromScript("IsInFistMode", &Victim, &None, 0))
            {
                if (Damager.CollisionShape.GetPhysicMaterial() != eEShapeMaterial_Metal
                    || (Victim.Inventory.GetItemFromSlot(gESlot_RightHand).CollisionShape.GetPhysicMaterial()
                        != eEShapeMaterial_Metal))
                {
                    EffectSystem::StartEffect("eff_col_weaponhitslevel_metal_wood_01", Victim);
                }
                else
                {
                    EffectSystem::StartEffect("eff_col_wh_01_me_me", Victim);
                }
            }

            gCScriptProcessingUnit::sAICombatMoveInstr_Args InstrArgs(DamagerOwner.GetInstance(), Victim.GetInstance(),
                                                                      DamagerOwnerAction, bCString("Recover"),
                                                                      1.5f); // g_pstrPhaseString[gEPhase_Recover]
            if (!gCScriptProcessingUnit::sAICombatMoveInstr(&InstrArgs, a_pSPU, GEFalse))
                return gEAction_ParadeStumble;

            if (HitForce > gEHitForce_Normal)
            {
                if (HitForce > gEHitForce_Heavy)
                {
                    Victim.Routine.FullStop();
                    Victim.Routine.SetTask("ZS_HeavyParadeStumble");
                    return gEAction_HeavyParadeStumble;
                }
                else
                {
                    Victim.Routine.FullStop();
                    Victim.Routine.SetTask("ZS_ParadeStumble");
                    return gEAction_ParadeStumble;
                }
            }

            Victim.Routine.FullStop();
            Victim.Routine.SetTask("ZS_ParadeStumble");
            return gEAction_QuickParadeStumble;
        }

        // Ausdauer und ggf. Lebenspunkte abziehen
        GEInt iStaminaRemaining =
            FinalDamage3 + ScriptAdmin.CallScriptFromScript("GetStaminaPoints", &Victim, &None, 0);
        if (iStaminaRemaining > 0)
            iStaminaRemaining = 0;
        ScriptAdmin.CallScriptFromScript("AddStaminaPoints", &Victim, &None, FinalDamage3);
        // Changed back the remaining raw Damage after Def. Reduction and Stamina consumption
        GEInt healthDamage = iStaminaRemaining * 2;
        if (FinalDamage != 0)
            healthDamage = iStaminaRemaining * 2 * FinalDamage2 / FinalDamage;
        ScriptAdmin.CallScriptFromScript("AddHitPoints", &Victim, &None, healthDamage);

        // Wenn der bei der Parade erhaltene Schaden das Opfer nicht unter 0 HP bringt
        if (ScriptAdmin.CallScriptFromScript("GetHitPoints", &Victim, &None, 0) > 0)
        {
            if (HitForce > gEHitForce_Normal)
            {
                if (HitForce > gEHitForce_Heavy)
                {
                    Victim.Routine.FullStop();
                    Victim.Routine.SetTask("ZS_HeavyParadeStumble");
                    return gEAction_HeavyParadeStumble;
                }
                else
                {
                    Victim.Routine.FullStop();
                    Victim.Routine.SetTask("ZS_ParadeStumble");
                    return gEAction_ParadeStumble;
                }
            }
            else
            {
                Victim.Routine.FullStop();
                Victim.Routine.SetTask("ZS_ParadeStumble");
                return gEAction_QuickParadeStumble;
            }
        }
    }

    // Reduce Damage for unkillable NPCs (obsolete)
    GEInt iVictimHitPoints = ScriptAdmin.CallScriptFromScript("GetHitPoints", &Victim, &None, 0);
    if (Victim != Player && !ScriptAdmin.CallScriptFromScript("CanBeKilled", &Victim, &None, 0))
    {
        if (FinalDamage2 > 30)
        {
            FinalDamage2 = 30;
        }
        // TODO: Invincible NPCs cannot be Killed atleast
        // They receive still some damage through blocks before
        // Atleast the engine does not let them get killed
        if (FinalDamage2 >= iVictimHitPoints)
        {
            FinalDamage2 = 0;
        }
    }

    // Modify damage in some situations, if AB is activated
    if (eCApplication::GetInstance().GetEngineSetup().AlternativeBalancing)
    {
        // Wenn es sich um einen Arenakampf handelt
        //  => Schaden = Schaden / 2
        /*if ((Victim != Player) && (Victim.NPC.GetProperty<PSNpc::PropertyAttackReason>() == gEAttackReason_Arena))
        {
        FinalDamage2 /= 2;
        }*/

        // Wenn der Held bei Schwierigkeitsgrad "hoch" eine Fernkampfwaffe benutzt
        //  => Schaden = Schaden * 1,2
        if (Victim != Player && IsNormalProjectileNB(Damager) && Entity::GetCurrentDifficulty() == EDifficulty_Hard)
        {
            FinalDamage2 = static_cast<GEInt>(FinalDamage2 * 1.2f);
        }
    }

    if (IsNormalProjectileNB(Damager) && ScriptAdmin.CallScriptFromScript("IsHumanoid", &Victim, &None, 0))
    {
        // Projectile is a Headbutt
        if (Damager.Damage.GetProperty<PSDamage::PropertyDamageType>() == gEDamageType_Impact)
        {
            if (ScriptAdmin.CallScriptFromScript("CanBeKilled", &Victim, &None, 0)
                && (ScriptAdmin.CallScriptFromScript("GetHitPoints", &Victim, &None, 0) - FinalDamage2) <= 0)
            {
                if (FinalDamage2 > 0)
                {
                    Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageAmount>() = FinalDamage2;
                    Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageType>() =
                        Damager.Damage.GetProperty<PSDamage::PropertyDamageType>();
                }
                if (ScriptAdmin.CallScriptFromScript("GetAttitude", &Victim, &DamagerOwner, 0)
                        == gEAttitude::gEAttitude_Hostile
                    || ScriptAdmin.CallScriptFromScript("GetAttitude", &Victim, &DamagerOwner, 0)
                           == gEAttitude::gEAttitude_Panic)
                {
                    Victim.Routine.FullStop();
                    Victim.Routine.SetTask("ZS_RagDollDead");
                    return gEAction_LieDead;
                }

                Victim.Routine.FullStop();
                Victim.Routine.SetTask("ZS_Unconscious");
                return gEAction_LieKnockDown;
            }

            if ((Damager.Damage.GetProperty<PSDamage::PropertyDamageHitMultiplier>()) >= 0.7f)
            {
                if (FinalDamage2 > 0)
                {
                    Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageAmount>() = FinalDamage2;
                    Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageType>() =
                        Damager.Damage.GetProperty<PSDamage::PropertyDamageType>();
                }
                Victim.Routine.FullStop();
                Victim.Routine.SetTask("ZS_SitKnockDown");
                return gEAction_SitKnockDown;
            }
        }
    }

    // Receive Damage (later outside of this function)
    if (FinalDamage2 > 0)
    {
        Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageAmount>() = FinalDamage2;
        Victim.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageType>() =
            Damager.Damage.GetProperty<PSDamage::PropertyDamageType>();
        if (pVictimDamageReceiver && pVictimDamageReceiver->IsValid())
        {
            pVictimDamageReceiver->SetLastHealthDamage(Entity::GetWorldEntity().Clock.GetTimeStampInSeconds());
        }
    }

    // Process StatusEffects and Animations:
    // CanBurn
    if (!Victim.NPC.IsFrozen() && ScriptAdmin.CallScriptFromScript("CanBurn", &Victim, &Damager, FinalDamage2))
    {
        Victim.NPC.EnableStatusEffects(gEStatusEffect_Burning, GETrue);
        Victim.Effect.StartRuntimeEffect("eff_magic_firespell_target_01");
    }
    // Dead, KnockDown or KnockedOut
    if ((iVictimHitPoints - FinalDamage2) <= 0)
    {
        if (ScriptAdmin.CallScriptFromScript("IsDeadlyDamage", &Victim, &Damager, 0))
        {
            Victim.Routine.FullStop();
            Victim.Routine.SetTask("ZS_RagDollDead");
            return gEAction_LieDead;
        }

        if (DamagerOwner.Routine.GetProperty<PSRoutine::PropertyAction>() == gEAction_PierceAttack
            && ScriptAdmin.CallScriptFromScript("IsHumanoid", &Victim, &None, 0))
        {
            Victim.Routine.FullStop();
            Victim.Routine.SetTask("ZS_PiercedKO");
            return gEAction_LiePiercedKO;
        }

        Victim.Routine.FullStop();
        if (HitForce >= 3)
        {
            Victim.Routine.SetTask("ZS_Unconscious");
            return gEAction_LieKnockOut;
        }
        Victim.Routine.SetTask("ZS_LieKnockDown");
        return gEAction_LieKnockDown;
    }

    if (ScriptAdmin.CallScriptFromScript("CanBePoisoned", &Victim, &Damager,
                                         DamagerOwnerAction == gEAction_PierceAttack
                                             || DamagerOwnerAction == gEAction_HackAttack))
    {
        if (pVictimDamageReceiver && pVictimDamageReceiver->IsValid())
        {
            pVictimDamageReceiver->AccessPoisonDamage() = GetPoisonDamage(DamagerOwner);
        }
        Victim.NPC.EnableStatusEffects(gEStatusEffect_Poisoned, GETrue);
    }

    if (ScriptAdmin.CallScriptFromScript("CanBeDiseased", &Victim, &Damager, 0))
    {
        Victim.NPC.EnableStatusEffects(gEStatusEffect_Diseased, GETrue);
    }

    if (ScriptAdmin.CallScriptFromScript("CanFreeze", &Victim, &Damager, FinalDamage2))
    {
        GEInt iFreezeTime = FinalDamage2 / 20;
        if (iFreezeTime < 5)
            iFreezeTime = 5;
        if (Damager.GetName() == "Mis_IceBlock")
            iFreezeTime = 30;
        Victim.Routine.FullStop();
        Victim.Routine.SetTask("ZS_Freeze");
        Victim.Routine.AccessProperty<PSRoutine::PropertyTaskPosition>() = 12 * iFreezeTime;
        return gEAction_None;
    }

    // Nostun Of animals when they are Mad
    if (!NBConfig::disableMonsterRage
        && Victim.Routine.GetProperty<PSRoutine::PropertyAction>() == gEAction_SprintAttack)
    {
        return gEAction_None;
    }

    // Stun Protection
    if (DamageTypeEntityTestNB(Victim, Damager) == VulnerabilityStatus_IMMUNE)
    {
        ScriptAdmin.CallScriptFromScript("AssessAgressor", &Victim, &DamagerOwner, 0);
        return gEAction_None;
    }

    if (VictimAction == gEAction_SitKnockDown)
    {
        ScriptAdmin.CallScriptFromScript("PipiStumble", &Victim, &None, 0);
        return gEAction_SitKnockDown;
    }

    if (DamagerOwner.Routine.GetProperty<PSRoutine::PropertyAction>() == gEAction_PierceAttack
        && ScriptAdmin.CallScriptFromScript("IsHumanoid", &Victim, &None, 0))
    {
        Victim.Routine.FullStop();
        Victim.Routine.SetTask("ZS_PierceStumble");
        return gEAction_PierceStumble;
    }
    // Freeze Reduced Timer on Hit

    // Magic Casting Protection!
    if (gEAction_Summon == Victim.Routine.GetProperty<PSRoutine::PropertyAction>()
        && 0.5f < Victim.Routine.GetStateTime() // Delay the protection a bit
        && !Victim.Interaction.GetSpell().GetName().Contains("Heal")
        && !Victim.Interaction.GetSpell().GetName().Contains("Cure"))
    {
        // If a special attack, or any strong attack-force is executed against the caster
        if (gEAction_HackAttack != DamagerOwnerAction && gEAction_PierceAttack != DamagerOwnerAction
            && NBConfig::KnockDownThreshold > HitForce && !isHeadshot)
        {
            ScriptAdmin.CallScriptFromScript("PipiStumble", &Victim, &None, 0); // Make Noice without Stumbles
            return gEAction_Stumble;
        }
    }

    // Troll are now more resistant to attacks but are a bit slower now
    if (Victim.NPC.GetProperty<PSNpc::PropertySpecies>() == gESpecies_Troll
        && (VictimAction == gEAction_PowerAttack || VictimAction == gEAction_SprintAttack
            || VictimAction == gEAction_Attack))
    {
        ScriptAdmin.CallScriptFromScript("PipiStumble", &Victim, &None, 0);
        return gEAction_QuickStumble;
    }

    // Scream or make HitEffect, but no Stumble also processes logic when you hit someone, like setting up combat mode
    if ((GEInt)HitForce <= gEHitForce_Minimal
        && (GetHeldWeaponCategoryNB(DamagerOwner) == gEWeaponCategory_Ranged || IsInActiveAttack(Victim)))
    {
        if (VictimAction == gEAction_PierceStumble)
        {
            Victim.Routine.FullStop();
            Victim.Routine.SetTask("ZS_QuickStumble");
            return gEAction_QuickStumble;
        }
        ScriptAdmin.CallScriptFromScript("PipiStumble", &Victim, &None, 0);
        return gEAction_QuickStumble;
    }

    if (GetHeldWeaponCategoryNB(Victim) != gEWeaponCategory_None
        && ScriptAdmin.CallScriptFromScript("IsHumanoid", &Victim, &None, 0))
    {
        if ((GEInt)HitForce >= NBConfig::
                KnockDownThreshold /* && GetHeldWeaponCategoryNB ( Victim ) == gEWeaponCategory_Melee */) // Remove
        {
            if (!NBConfig::onlyHeavyAttackKnockDown || DamagerOwnerAction == gEAction_HackAttack
                || DamagerOwnerAction == gEAction_PowerAttack || DamagerOwnerAction == gEAction_WhirlAttack)
            {
                Victim.Routine.FullStop();
                Victim.Routine.SetTask("ZS_SitKnockDown");
                return gEAction_SitKnockDown;
            }
        }

        if (HitForce < gEHitForce_Heavy)
        {
            Victim.Routine.FullStop();
            Victim.Routine.SetTask("ZS_QuickStumble");
            return gEAction_QuickStumble;
        }
    }

    Victim.Routine.FullStop();
    Victim.Routine.SetTask("ZS_Stumble");
    return gEAction_Stumble;
}
