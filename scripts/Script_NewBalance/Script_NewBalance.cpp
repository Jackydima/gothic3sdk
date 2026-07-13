#include "Script_NewBalance.h"

// eSSetupEngine[1ea] = AB; eSSetupEngine[1eb] alternative AI

gSScriptInit &GetScriptInit()
{
    static gSScriptInit s_ScriptInit;
    return s_ScriptInit;
}

void LoadSettings()
{
    eCConfigFile config = eCConfigFile();
    if (config.ReadFile(bCString("monsterdamage.ini")))
    {
        NBConfig::fMonsterDamageMultiplicator = config.GetFloat(
            bCString("Game"), bCString("Game.MonsterDamageMultiplicator"), NBConfig::fMonsterDamageMultiplicator);
    }
    if (config.ReadFile("newbalance.ini"))
    {
        NBConfig::newSummoning = config.GetBool("Script", "NewSummoning", NBConfig::newSummoning);
        NBConfig::vanishSummons = config.GetBool("Script", "VanishSummons", NBConfig::vanishSummons);
        NBConfig::onlyHeavyAttackKnockDown =
            config.GetBool("Script", "OnlyHeaveAttackKnockDown", NBConfig::onlyHeavyAttackKnockDown);
        NBConfig::useNewBalanceMagicWeapon =
            config.GetBool("Script", "UseNewBalanceMagicWeapon", NBConfig::useNewBalanceMagicWeapon);
        NBConfig::useExtendedBlocking = config.GetBool("Script", "UseExtendedBlocking", NBConfig::useExtendedBlocking);
        NBConfig::useHardCoreAttacks = config.GetBool("Script", "UseHardCoreAttacks", NBConfig::useHardCoreAttacks);
        NBConfig::useDamagingInnosLight =
            config.GetBool("Script", "UseDamagingInnosLight", NBConfig::useDamagingInnosLight);
        NBConfig::useNewStaminaMechanic =
            config.GetBool("Script", "UseNewStaminaMechanic", NBConfig::useNewStaminaMechanic);
        NBConfig::useAlwaysMaxLevel = config.GetBool("Script", "DisableNPCLeveling", NBConfig::useAlwaysMaxLevel);
        NBConfig::enablePerfectBlock = config.GetBool("Script", "EnablePerfectBlock", NBConfig::enablePerfectBlock);
        NBConfig::playerOnlyPerfectBlock =
            config.GetBool("Script", "PlayerOnlyPerfectBlock", NBConfig::playerOnlyPerfectBlock);
        NBConfig::useNewBalanceMeleeScaling =
            config.GetBool("Script", "NewMeleeScaling", NBConfig::useNewBalanceMeleeScaling);
        NBConfig::adjustXPReceive = config.GetBool("Script", "AdjustXPReceive", NBConfig::adjustXPReceive);
        NBConfig::useStaticBlocks = config.GetBool("Script", "UseStaticBlocks", NBConfig::useStaticBlocks);
        NBConfig::useStrengthForCrossbows =
            config.GetBool("Script", "UseStrengthForCrossbows", NBConfig::useStrengthForCrossbows);

        NBConfig::summoningLevelMultiplier =
            config.GetFloat("Script", "SummoningLevelMultiplier", NBConfig::summoningLevelMultiplier);

        NBConfig::fEvadeDistance = config.GetFloat("Script", "EvadeDistance", NBConfig::fEvadeDistance);
        NBConfig::fEvadeSpeed = config.GetFloat("Script", "EvadeSpeed", NBConfig::fEvadeSpeed);
        NBConfig::fParrySpeed = config.GetFloat("Script", "ParrySpeed", NBConfig::fParrySpeed);

        NBConfig::PerfectBlockDamageMult =
            config.GetFloat("Script", "PerfectBlockDamageMult", NBConfig::PerfectBlockDamageMult);
        NBConfig::PowerAttackArmorPen = config.GetFloat("Script", "PowerAttackArmorPen", NBConfig::PowerAttackArmorPen);
        NBConfig::QuickAttackArmorRes = config.GetFloat("Script", "QuickAttackArmorRes", NBConfig::QuickAttackArmorRes);
        NBConfig::SpecialAttackArmorPen =
            config.GetFloat("Script", "SpecialAttackArmorPen", NBConfig::SpecialAttackArmorPen);
        NBConfig::NPCStrengthMultiplicator =
            config.GetFloat("Script", "NPCStrengthMultiplicator", NBConfig::NPCStrengthMultiplicator);
        NBConfig::NPCStrengthAddition =
            config.GetFloat("Script", "NPCStrengthAddition", NBConfig::NPCStrengthAddition);
        NBConfig::elementalPerkBonusResistance =
            config.GetInt("Script", "ElementalPerkBonusResistance", NBConfig::elementalPerkBonusResistance);
        NBConfig::animationSpeedBonusMid =
            config.GetFloat("Script", "BowAnimationSpeedBonusMid", NBConfig::animationSpeedBonusMid);
        NBConfig::animationSpeedBonusHigh =
            config.GetFloat("Script", "BowAnimationSpeedBonusHigh", NBConfig::animationSpeedBonusHigh);

        NBConfig::NPCDamageReductionMultiplicator =
            config.GetFloat("Script", "NPCDamageReductionMultiplicator", NBConfig::NPCDamageReductionMultiplicator);
        NBConfig::poiseThreshold = config.GetInt("Script", "PoiseThreshold", NBConfig::poiseThreshold);
        NBConfig::MonsterRageModus = config.GetInt("Script", "MonsterRageModus", NBConfig::MonsterRageModus);
        NBConfig::staminaRecoveryDelay =
            config.GetU32("Script", "StaminaRecoveryDelay", NBConfig::staminaRecoveryDelay);
        NBConfig::staminaRecoveryPerTick =
            config.GetInt("Script", "StaminaRecoveryPerTick", NBConfig::staminaRecoveryPerTick);
        NBConfig::npcArmorMultiplier = static_cast<GEDouble>(
            config.GetFloat("Script", "NPCProtectionMultiplier", static_cast<GEFloat>(NBConfig::npcArmorMultiplier)));
        NBConfig::playerArmorMultiplier =
            config.GetFloat("Script", "PlayerProtectionMultiplier", NBConfig::playerArmorMultiplier);
        NBConfig::npcWeaponDamageMultiplier =
            config.GetFloat("Script", "NPCWeaponDamageMultiplier", NBConfig::npcWeaponDamageMultiplier);
        NBConfig::useNewBowMechanics = config.GetBool("Script", "NewBowMechanics", NBConfig::useNewBowMechanics);
        NBConfig::attackRangeAI = static_cast<GEDouble>(
            config.GetFloat("Script", "AttackRangeAI", static_cast<GEFloat>(NBConfig::attackRangeAI)));
        NBConfig::telekinesisRange = static_cast<GEDouble>(
            config.GetFloat("Script", "TelekinesisRange", static_cast<GEFloat>(NBConfig::telekinesisRange)));
        NBConfig::shootVelocity = static_cast<GEDouble>(
            config.GetFloat("Script", "ProjectileVelocity", static_cast<GEFloat>(NBConfig::shootVelocity)));
        NBConfig::NPC_AIM_INACCURACY = config.GetFloat("Script", "NPCAimInaccuracy", NBConfig::NPC_AIM_INACCURACY);
        NBConfig::ATTACK_REACH_MULTIPLIER =
            config.GetFloat("Script", "AttackReachMultiplier", NBConfig::ATTACK_REACH_MULTIPLIER);
        NBConfig::startSTR = config.GetInt("Script", "StartSTR", NBConfig::startSTR);
        NBConfig::startDEX = config.GetInt("Script", "StartDEX", NBConfig::startDEX);
        NBConfig::blessedBonus = config.GetInt("Script", "BlessedBonus", NBConfig::blessedBonus);
        NBConfig::sharpBonus = config.GetInt("Script", "SharpBonus", NBConfig::sharpBonus);
        NBConfig::useSharpPercentage = config.GetBool("Script", "UseSharpPercentage", NBConfig::useSharpPercentage);
        NBConfig::forgedBonus = config.GetInt("Script", "ForgedBonus", NBConfig::forgedBonus);
        NBConfig::wornPercentageMalus = config.GetInt("Script", "WornMalus", NBConfig::wornPercentageMalus);
        NBConfig::npcArenaSpeedMultiplier =
            config.GetFloat("Script", "NPCArenaSpeedMultiplier", NBConfig::npcArenaSpeedMultiplier);
        NBConfig::enableNPCSprint = config.GetBool("Script", "EnableNPCSprint", NBConfig::enableNPCSprint);
        NBConfig::zombiesCanSprint = config.GetBool("Script", "ZombiesCanSprint", NBConfig::zombiesCanSprint);
        NBConfig::enableNewTransformation =
            config.GetBool("Script", "EnableNewTransformation", NBConfig::enableNewTransformation);
        NBConfig::disableMonsterRage = config.GetBool("Script", "DisableMonsterRage", NBConfig::disableMonsterRage);
        NBConfig::enableNewMagicAiming =
            config.GetBool("Script", "EnableNewMagicAiming", NBConfig::enableNewMagicAiming);
        NBConfig::bEnableEvading = config.GetBool("Script", "EnableEvading", NBConfig::bEnableEvading);
        NBConfig::bEnableEvadeWithJump =
            config.GetBool("Script", "EnableEvadeWithJump", NBConfig::bEnableEvadeWithJump);
        NBConfig::bEnableParry = config.GetBool("Script", "EnableParry", NBConfig::bEnableParry);
        NBConfig::enableAOEDamage = config.GetBool("Script", "EnableAOEDamage", NBConfig::enableAOEDamage);
        bCString AOENamesString = config.GetString("Script", "AOENames", "");
        NBConfig::AOENames = splitTobCStrings(AOENamesString.GetText(), ',');
        NBConfig::bossLevel = config.GetInt("Script", "BossLevelCap", NBConfig::bossLevel);
        NBConfig::uniqueLevel = config.GetInt("Script", "UniqueLevelCap", NBConfig::uniqueLevel);
        NBConfig::eliteLevel = config.GetInt("Script", "EliteLevelCap", NBConfig::eliteLevel);
        NBConfig::warriorLevel = config.GetInt("Script", "WarriorLevelCap", NBConfig::warriorLevel);
        NBConfig::noviceLevel = config.GetInt("Script", "NoviceLevelCap", NBConfig::noviceLevel);
        NBConfig::KnockDownThreshold = config.GetInt("Script", "KnockDownThreshold", NBConfig::KnockDownThreshold);
        NBConfig::DiseaseNPCDuration = config.GetInt("Script", "DiseaseNPCDuration", NBConfig::DiseaseNPCDuration);
        NBConfig::DiseasePlayerDuration = config.GetInt("Script", "DiseasePlayerDuration", NBConfig::DiseasePlayerDuration);
    }
}

void AssignNewKeys()
{
    static gCSessionKeys sessionKeys = gCSessionKeys();
    sessionKeys = gCSession::GetInstance().GetSessionKeys();

    // Getting Keys at
    // https://github.com/Jackydima/gothic3sdk/blob/master/g3/Engine/include/g3sdk/Engine/io/ge_inpshared.h#L9

    eCConfigFile config = eCConfigFile();
    if (config.ReadFile(bCString("newbalance.ini")))
    {
        /*[SessionKey.Parry]
        Key1.Type=1
        Key1.Offset=6  // Mouse4
        Key2.Type=-1
        Key2.Offset=-1*/

        bCString keyName = "Parry";
        static eSSetupEngine::SPhysicalKeys pKeys;
        pKeys.m_iKey1DeviceType = config.GetInt("SessionKey.Parry", "Key1.Type", eEDeviceType_Mouse);
        pKeys.m_iKey1DeviceOffset =
            config.GetInt("SessionKey.Parry", "Key1.Offset", eCInpShared::eEMouseOffset_Button3);
        pKeys.m_iKey2DeviceType = config.GetInt("SessionKey.Parry", "Key2.Type", -1);
        pKeys.m_iKey2DeviceOffset = config.GetInt("SessionKey.Parry", "Key2.Offset", -1);

        mCCaller CallerAssignSingleKey(
            mCCaller::GetCallerParams(RVA_Game(0x1829d0), mERegisterType::mERegisterType_Ecx));
        using AssignSingleKey_t = void(GE_STDCALL *)(gESessionKey, bCString, eSSetupEngine::SPhysicalKeys *);
        CallerAssignSingleKey.SetEcx(&sessionKeys);
        CallerAssignSingleKey.GetFunction<AssignSingleKey_t>()(gESessionKey_Parry, keyName, &pKeys);
    }
}

void AddNewEffect()
{
    // EffectModulePtr
    DWORD EffectModulePtr = ((DWORD (*)(void))(RVA_Game(0x601f0)))();
    if (EffectModulePtr == 0)
        return;
    DWORD gCEffectSystemPtr = EffectModulePtr + 0x14;
    if (gCEffectSystemPtr == 0)
        return;
    gCEffectMap *EffectMap = (gCEffectMap *)(*(DWORD *)gCEffectSystemPtr + 0x4);
    if (EffectMap == nullptr)
        return;
    gCEffectMap NewEM;
    NewEM.Load("g3-new.efm");
    for (auto iter = NewEM.Begin(); iter != NewEM.End(); iter++)
    {
        EffectMap->RemoveAt(iter.GetKey());
        gCEffectCommandSequence *effectCommand = EffectMap->InsertNewAt(iter.GetKey());
        *effectCommand = iter.GetNode()->m_Element;
    }
}

extern "C" __declspec(dllexport) gSScriptInit const *GE_STDCALL ScriptInit(void)
{
    // Ensure that that Script_Game.dll is loaded.
    GetScriptAdmin().LoadScriptDLL("Script_Game.dll");
    LoadSettings();
    AssignNewKeys();
    PatchCode();
    AddNewEffect();
    if (NBConfig::enableNewMagicAiming)
    {
        InitGUI();
    }

    mCModuleRegistry::GetInstance().Apply();

    HookFunctions();
    HookCallHooks();

    static mCFunctionHook Hook_Assesshit;
    Hook_Assesshit.Hook(GetScriptAdminExt().GetScript("AssessHit")->m_funcScript, &AssessHit,
                        mCBaseHook::mEHookType_OnlyStack);

    return &GetScriptInit();
}

//
// Entry Point
//

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
#ifdef GE_DEBUG
            AllocConsole();
            freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
#endif
            ::DisableThreadLibraryCalls(hModule);
            break;
        case DLL_PROCESS_DETACH: break;
    }
    return TRUE;
}
