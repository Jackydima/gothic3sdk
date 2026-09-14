#include "SharedConfig.h"

namespace NBConfig
{

std::map<bCString, GEInt> NpcHealthMap = {};

GEInt bossLevel = 65;
GEInt uniqueLevel = 45;
GEInt eliteLevel = 35;
GEInt warriorLevel = 30;
GEInt noviceLevel = 20;

GEInt KnockDownThreshold = 5;
GEInt MonsterRageModus = 2;

GEFloat summoningLevelMultiplier = 5.0f;

GEBool UseNewDamageCalculation = GETrue;
GEBool UseNewNPCProtection = GETrue;
GEBool IgnoreHumanoidBodyProtection = GETrue;
GEBool ZeroStaminaAttack = GEFalse;
GEBool newSummoning = GETrue;
GEBool vanishSummons = GETrue;
GEBool onlyHeavyAttackKnockDown = GEFalse;
GEBool useHardCoreAttacks = GETrue;
GEBool useExtendedBlocking = GEFalse;
GEBool useDamagingInnosLight = GETrue;
GEBool useNewBalanceMagicWeapon = GEFalse;
GEBool useNewStaminaMechanic = GETrue;
GEBool useAlwaysMaxLevel = GEFalse;
GEBool enablePerfectBlock = GETrue;
GEBool playerOnlyPerfectBlock = GEFalse;
GEBool useNewBalanceMeleeScaling = GETrue;
GEBool adjustXPReceive = GETrue;
GEBool useStaticBlocks = GEFalse;
GEBool useStrengthForCrossbows = GEFalse;
GEBool useNewBowMechanics = GETrue;
GEBool enableNPCSprint = GETrue;
GEBool zombiesCanSprint = GEFalse;
GEBool enableNewTransformation = GETrue;
GEBool disableMonsterRage = GETrue;
GEBool enableNewMagicAiming = GETrue;
GEBool enableAOEDamage = GETrue;
GEBool bEnableEvading = GETrue;
GEBool bEnableEvadeWithJump = GETrue;
GEBool bEnableParry = GETrue;
std::vector<bCString> AOENames = {};

GEFloat fEvadeDistance = 220.0f;
GEFloat fEvadeSpeed = 1.5f;
GEFloat fParrySpeed = 1.0f;

GEFloat MissileAttackArmorPen = 0.0f;
GEFloat VulnerabilityWeak = 1.6f;
GEFloat VulnerabilitySlightlyWeak = 1.2f;
GEFloat VulnerabilityStrong = 0.5f;
GEFloat VulnerabilitySlightlyStrong = 0.8f;

GEFloat PerfectBlockDamageMult = 1.5f;
GEFloat PowerAttackArmorPen = 0.1f;
GEFloat QuickAttackArmorRes = 0.05f;
GEFloat SpecialAttackArmorPen = 0.115f;
GEFloat NPCStrengthMultiplicator = 2.25f;
GEFloat NPCStrengthAddition = 20.0f;
GEFloat NPCDamageReductionMultiplicator = 0.25f;
GEInt poiseThreshold = -1;
GEFloat npcArenaSpeedMultiplier = 1.1f;
GEFloat npcWeaponDamageMultiplier = 1.0f;
GEU32 staminaRecoveryDelay = 15;
GEInt staminaRecoveryPerTick = 25;
GEFloat fMonsterDamageMultiplicator = 0.5f; // Default 0.5
GEDouble npcArmorMultiplier = 1.5;
GEDouble npcArmorMultiplierAbsolute = 2.0;
GEFloat playerArmorMultiplier = 0.4f;
GEInt startSTR = 0;
GEInt startDEX = 0;
GEDouble attackRangeAI = 3000.0;
GEDouble telekinesisRange = 4000.0;
GEDouble shootVelocity = 300.0;
GEFloat ReflectFOV = 5.0f;
GEFloat NPC_AIM_INACCURACY = 0.40f;
GEFloat ATTACK_REACH_MULTIPLIER = 1.0f;
GEInt elementalPerkBonusResistance = 25;
GEFloat animationSpeedBonusMid = 1.5f;
GEFloat animationSpeedBonusHigh = 2.0f;

GEInt blessedBonus = 10;
GEInt sharpBonus = 10;
GEBool useSharpPercentage = GETrue;
GEInt forgedBonus = 20;
GEInt wornPercentageMalus = 70;
GEInt healthRecoveryDelay = 60;
GEInt DiseaseNPCDuration = 12*1*60; // 12 Minutes Ingame = 1 Minute real time
GEInt DiseasePlayerDuration = 8*60*60; // 8 Hours Ingame -> Sleeping possible, or wait like 45 minutes
} // namespace NBConfig
