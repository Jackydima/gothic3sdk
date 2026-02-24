#pragma once

#include "vector"
#include <g3sdk/util/Memory.h>
#include <g3sdk/util/Logging.h>
#include <g3sdk/util/Hook.h>


//TODO 
extern GEFloat npcSTRMultiplier;
extern GEInt npcSTRAddition;
//
// 
// Configureable

//NPC Level for Powergroup
extern GEInt bossLevel;
extern GEInt uniqueLevel;
extern GEInt eliteLevel;
extern GEInt warriorLevel;
extern GEInt noviceLevel;

extern GEInt KnockDownThreshold;
extern GEInt MonsterRageModus;

extern GEFloat summoningLevelMultiplier;

extern GEBool newSummoning;
extern GEBool vanishSummons;
extern GEBool onlyHeaveAttackKnockDown;
extern GEBool useExtendedBlocking;
extern GEBool useHardCoreAttacks;
extern GEBool useDamagingInnosLight;
extern GEBool useNewBalanceMagicWeapon;
extern GEBool useNewStaminaRecovery;
extern GEBool useAlwaysMaxLevel;
extern GEBool enablePerfectBlock;
extern GEBool playerOnlyPerfectBlock;
extern GEBool useNewBalanceMeleeScaling;
extern GEBool adjustXPReceive;
extern GEBool useStaticBlocks;
extern GEBool useStrengthForCrossbows;
extern GEBool alternativeProtection;
extern GEBool useNewBowMechanics;
extern GEBool enableNPCSprint;
extern GEBool zombiesCanSprint;
extern GEBool enableNewTransformation;
extern GEBool disableMonsterRage;
extern GEBool enableNewMagicAiming;
extern GEBool enableAOEDamage;
extern std::vector<bCString> AOENames;

extern GEFloat PerfectBlockDamageMult;
extern GEFloat PowerAttackArmorPen;
extern GEFloat QuickAttackArmorRes;
extern GEFloat SpecialAttackArmorPen;
extern GEFloat NPCStrengthMultiplicator;
extern GEFloat NPCStrengthCorrection;
extern GEFloat NPCDamageReductionMultiplicator;
extern GEInt poiseThreshold;
extern GEFloat npcArenaSpeedMultiplier;
extern GEFloat npcWeaponDamageMultiplier;
extern GEInt staminaRecoveryDelay;
extern GEInt staminaRecoveryPerTick;
extern GEFloat fMonsterDamageMultiplicator; // Default 0.5
extern GEDouble npcArmorMultiplier;
extern GEFloat playerArmorMultiplier;
extern GEInt startSTR;
extern GEInt startDEX;
extern GEDouble attackRangeAI;
extern GEDouble telekinesisRange;
extern GEDouble shootVelocity;
extern GEFloat NPC_AIM_INACCURACY;
extern GEFloat ATTACK_REACH_MULTIPLIER;
extern GEInt elementalPerkBonusResistance;

extern GEFloat animationSpeedBonusMid;
extern GEFloat animationSpeedBonusHigh;

extern GEInt blessedBonus;
extern GEInt sharpBonus;
extern GEBool useSharpPercentage;
extern GEInt forgedBonus;
extern GEInt wornPercentageMalus;

//
// 
//Const Data for Usage:
static const GEDouble PROJECTILEMULTIPLICATOR = 27.777778;
static const GEDouble* npcArmorMultiplierPtr = &npcArmorMultiplier;
static const GEDouble* shootVelocityPtr = &shootVelocity;
static const GEDouble* attackRangeAIPtr = &attackRangeAI;
static const GEDouble* telekinesisRangePtr = &telekinesisRange;
//static GEFloat animationSpeedBonusMid = 1.5f;
//static GEFloat animationSpeedBonusHigh = 2.0f;

static char* BONETARGET = "Spine_Spine_1";
static char* powerAimEffectName = "eff_ani_fight_bow_raise_01";
static char* powerAimWarriorEffectName = "eff_ani_fight_bow_raise_warrior_01";
static char* powerAimEliteEffectName = "eff_ani_fight_bow_raise_elite_01";
static char* crosshairName = "G3_HUD_CrossHair.tga";

static bCString blessedBonusString;
static bCString sharpBonusString;
static bCString forgedBonusString;
static bCString wornMalusString;
//