#pragma once
#include <iostream>
#include <g3sdk/Script.h>
#include <g3sdk/util/Memory.h>
#include <g3sdk/util/Logging.h>
#include <g3sdk/util/Hook.h>
#include <g3sdk/util/Util.h>
#include <g3sdk/util/ScriptUtil.h>
#include <map>

#include "SharedConfig.h"


#ifndef GE_DEBUG
template <typename... Args>
void print(const char *format, Args... args)
{}

template <typename... Args>
void println(const char *format, Args... args)
{}
#else
template <typename... Args>
void print(const char *format, Args... args)
{
    std::fprintf(stdout, format, args...);
}
template <typename... Args>
void println(const char *format, Args... args)
{
    std::fprintf(stdout, format, args...);
    std::fprintf(stdout, "\n");
}
#endif

enum VulnerabilityStatus {
	VulnerabilityStatus_IMMUNE = 0,
	VulnerabilityStatus_REGULAR = 1,
	VulnerabilityStatus_WEAK = 2,
	VulnerabilityStatus_STRONG = 3,
	VulnerabilityStatus_SLIGHTLYWEAK = 4,
	VulnerabilityStatus_SLIGHTLYSTRONG = 5,
};

enum WarriorType {
	WarriorType_None = 0,
	WarriorType_Novice = 1,
	WarriorType_Warrior = 2,
	WarriorType_Elite = 3,
};


std::vector<bCString> splitTobCStrings ( const std::string str , char delim );
void MagicPartyMemberRemover ( Entity p_summoner );
void VanishEntity ( Entity& p_entity );
void DoAOEDamage ( Entity& p_damager , Entity& p_victim );
GEBool CanRage ( Entity& p_entity );
GEBool IsInActiveAttack ( Entity& p_entity );
void PartyMonsterSpawn ( Entity& p_summoner, Template& p_summonTemplate, GEInt p_int1, GEBool suppressEffect, GEFloat multiplicator = 1.0f );
GEInt getPowerLevel ( Entity& p_entity );
gEWeaponCategory GetHeldWeaponCategoryNB ( Entity const& a_Entity );
Template getProjectile ( Entity& p_entity , gEUseType p_rangedWeaponType );
GEBool isBigMonster ( Entity& p_monster );
GEInt getHyperArmorPoints ( Entity& p_monster , gEAction p_monsterAction );
GEInt IsEvil ( gCScriptProcessingUnit* a_pSPU , Entity* a_pSelfEntity , Entity* a_pOtherEntity , GEU32 a_iArgs );
GEInt CanBurn ( gCScriptProcessingUnit* a_pSPU , Entity* a_pSelfEntity , Entity* a_pOtherEntity , GEU32 a_iArgs );
GEInt CanFreeze ( gCScriptProcessingUnit* a_pSPU , Entity* a_pSelfEntity , Entity* a_pOtherEntity , GEU32 a_iArgs );
GEInt CanBePoisoned ( gCScriptProcessingUnit* a_pSPU , Entity* a_pSelfEntity , Entity* a_pOtherEntity , GEU32 a_iArgs );
GEBool IsNormalProjectileNB ( Entity& p_damager );
GEBool IsSpellContainerNB ( Entity& p_damager );
GEBool IsMagicProjectileNB ( Entity& p_damager );
GEBool CheckHandUseTypesNB ( gEUseType p_lHand , gEUseType p_rHand , Entity& entity );
GEInt GetSkillLevelsNB ( Entity& p_entity );
GEInt GetActionWeaponLevelNB ( Entity& p_damager , gEAction p_action );
GEInt GetShieldLevelBonusNB ( Entity& p_entity );
VulnerabilityStatus DamageTypeEntityTestNB ( Entity& p_victim , Entity& p_damager );
GEInt GetHyperActionBonus ( gEAction p_action );
GEU32 GetPoisonDamage ( Entity& attacker );
GEInt getWeaponLevelNB ( Entity& p_entity );
GEBool IsHoldingTwoHandedWeapon ( Entity& entity );
GEBool IsInSameParty ( Entity& p_self , Entity& p_other );
GEBool IsPlayerInCombat ( );
GEBool IsInRecovery ( Entity& p_entity );
GEInt speciesLeftHand ( Entity p_entity );
GEInt speciesRightHand ( Entity p_entity );
WarriorType GetWarriorType ( Entity& p_entity );
GEInt getLastTimeFromMap ( bCString iD , std::map<bCString , GEInt>& map );