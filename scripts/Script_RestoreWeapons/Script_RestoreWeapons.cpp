#include "Script_RestoreWeapons.h"

gSScriptInit &GetScriptInit()
{
    static gSScriptInit s_ScriptInit;
    return s_ScriptInit;
}

static RWConfig rwConfig = RWConfig();

void LoadSettings()
{
    eCConfigFile config = eCConfigFile();
    if (config.ReadFile(bCString("RestoreWeapons.ini")))
    {
        rwConfig.G_Mode = config.GetU32("Main", "Mode", 0);
        rwConfig.ReqItemAmount = config.GetU32("Main", "ReqItemAmount", 1);
        rwConfig.ReqGoldMultiplicator = config.GetFloat("Main", "ReqGoldMultiplicator", 1.0f);
        if (rwConfig.ReqGoldMultiplicator < 0)
        {
            rwConfig.ReqGoldMultiplicator *= -1;
        }
    }
}

/*GEInt ModifyQuality(gEItemQuality desiredQualityAddition, GEInt inventoryIndex, Entity &p_entity)
{
    gEItemCategory category = p_entity.Inventory.GetCategory(inventoryIndex);
    if (category == gEItemCategory_Armor)
        return 1;
    gEItemQuality itemQuality = (gEItemQuality)p_entity.Inventory.GetQuality(inventoryIndex);
    if ((itemQuality & gEItemQuality_Burning) == gEItemQuality_Burning
        || (itemQuality & gEItemQuality_Frozen) == gEItemQuality_Frozen)
    {
        gui2.PrintGameMessage(eCLocString("HUD_AlreadyMagic").GetString(), gEGameMessageType_Failure);
        return GetScriptAdmin().CallScriptFromScript("StartSayDoesntWork", &p_entity, &p_entity) & 0xFFFFFF00;
    }
    GEInt stackItemIndex = -1;
    switch (desiredQualityAddition)
    {
        case gEItemQuality_Blessed:
            if ((itemQuality & gEItemQuality_Blessed) == gEItemQuality_Blessed)
            {
                gui2.PrintGameMessage(eCLocString("HUD_AlreadyBlessed").GetString(), gEGameMessageType_Failure);
                return GetScriptAdmin().CallScriptFromScript("StartSayDoesntWork", &p_entity, &p_entity) & 0xFFFFFF00;
            }
            if (*(BYTE *)RVA_ScriptGame(0x118a90))
            {
                GEInt currentManaPoints = GetScriptAdmin().CallScriptFromScript("GetManaPoints", &p_entity, &None);
                if (currentManaPoints < 100)
                {
                    // bCUnicodeString uString = bCUnicodeString::GetFormattedString(L"%1d");
                    bCUnicodeString uString = eCLocString("HUD_MissingAttrib");
                    uString.Replace(L"$(value)", bCUnicodeString::GetFormattedString(L"%ld", 100 - currentManaPoints));
                    uString.Replace(L"$(name)", eCLocString("ATTRIB_MP").GetString());
                    gui2.PrintGameMessage(uString, gEGameMessageType_Failure);
                    return GetScriptAdmin().CallScriptFromScript("StartSayDoesntWork", &p_entity, &p_entity)
                         & 0xFFFFFF00;
                }
                GetScriptAdmin().CallScriptFromScript("AddManaPoints", &p_entity, &p_entity, -100);
            }
            else
            {
                stackItemIndex = p_entity.Inventory.FindStackIndex(Template("It_Scroll_BlessWeapon"));
            }
            EffectSystem::StartEffect("eff_inv_bless_01", p_entity);
            p_entity.Inventory.ApplyQuality(inventoryIndex, desiredQualityAddition, 1);
            break;

        case gEItemQuality_Sharp:
            if (!p_entity.Inventory.IsSkillActive("Perk_SharpenBlade"))
            {
                bCUnicodeString uString = eCLocString("HUD_MissingSkill");
                uString.Replace(L"$(name)", eCLocString("FO_It_Perk_SharpenBlade").GetString());
                gui2.PrintGameMessage(uString, gEGameMessageType_Failure);
                return GetScriptAdmin().CallScriptFromScript("StartSayDoesntWork", &p_entity, &p_entity) & 0xFFFFFF00;
            }
            print("Config: rwConfig.G_Mode=%d, rwConfig.ReqItemAmount=%d, rwConfig.ReqGoldMultiplicator=%f\n",
                  rwConfig.G_Mode, rwConfig.ReqItemAmount, rwConfig.ReqGoldMultiplicator);
            if ((itemQuality & gEItemQuality_Worn) == gEItemQuality_Worn)
            {
                if (rwConfig.G_Mode == 0)
                {
                    GEInt amount = p_entity.Inventory.GetAmount(inventoryIndex);
                    Entity item = p_entity.Inventory.GetTemplateItem(inventoryIndex);
                    if (amount < 1 + rwConfig.ReqItemAmount)
                    {
                        bCUnicodeString uString = eCLocString("HUD_MissingItem").GetString();
                        uString.Replace(L"$(value)", bCUnicodeString::GetFormattedString(
                                                         L"%ld", 1 + rwConfig.ReqItemAmount - amount));
                        bCString entityNameLoc = "FO_" + item.GetName();
                        uString.Replace(L"$(name)", eCLocString(entityNameLoc).GetString());
                        uString.Replace(L"$(item)", eCLocString(entityNameLoc).GetString());
                        gui2.PrintGameMessage(uString, gEGameMessageType_Failure);
                        return GetScriptAdmin().CallScriptFromScript("StartSayDoesntWork", &p_entity, &p_entity)
                             & 0xFFFFFF00;
                    }
                    bCUnicodeString uString = eCLocString("GO_ItemGiven").GetString();
                    uString =
                        bCUnicodeString::GetFormattedString(L"%ld x %s %s", rwConfig.ReqItemAmount,
                                                            eCLocString("FO_" + item.GetName()).GetString(), uString);
                    gui2.PrintGameMessage(uString, gEGameMessageType_Success);
                    p_entity.Inventory.DeleteItems(inventoryIndex, rwConfig.ReqItemAmount);
                }
                else
                {
                    Entity templateEntity = p_entity.Inventory.GetTemplateItem(inventoryIndex);
                    GEInt goldValue =
                        static_cast<GEInt>((GEFloat)templateEntity.Item.GetGoldValue() * rwConfig.ReqGoldMultiplicator);
                    GEInt goldIndex = p_entity.Inventory.FindStackIndex("It_Gold");
                    GEInt goldAmount = p_entity.Inventory.GetAmount(goldIndex);
                    print("Weapon Value: %d\nGold Amount: %d\n", goldValue, goldAmount);
                    if (goldAmount < goldValue)
                    {
                        bCUnicodeString uString = eCLocString("HUD_MissingItem").GetString();
                        uString.Replace(L"$(value)",
                                        bCUnicodeString::GetFormattedString(L"%ld", goldValue - goldAmount));
                        Entity item = p_entity.Inventory.GetTemplateItem(inventoryIndex);
                        Entity gold = p_entity.Inventory.GetTemplateItem(goldIndex);
                        bCString entityItemNameLoc = "FO_" + item.GetName();
                        uString.Replace(L"$(name)", eCLocString(entityItemNameLoc).GetString());
                        bCString entityGoldNameLoc = "FO_" + gold.GetName();
                        uString.Replace(L"$(item)", eCLocString(entityGoldNameLoc).GetString());
                        gui2.PrintGameMessage(uString, gEGameMessageType_Failure);
                        return GetScriptAdmin().CallScriptFromScript("StartSayDoesntWork", &p_entity, &p_entity)
                             & 0xFFFFFF00;
                    }
                    //"GO_GoldGiven"
                    bCUnicodeString uString = eCLocString("GO_GoldGiven").GetString();
                    uString = bCUnicodeString::GetFormattedString(L"%ld x %s", goldValue, uString);
                    gui2.PrintGameMessage(uString, gEGameMessageType_Success);
                    p_entity.Inventory.DeleteItems(goldIndex, goldValue);
                }
                EffectSystem::StartEffect("eff_inv_sharpen_01", p_entity);
                p_entity.Inventory.SetQuality(inventoryIndex,
                                              (p_entity.Inventory.GetQuality(inventoryIndex) & ~gEItemQuality_Worn), 1);
                break;
            }

            if ((itemQuality & gEItemQuality_Sharp) == gEItemQuality_Sharp)
            {
                gui2.PrintGameMessage(eCLocString("HUD_AlreadySharp").GetString(), gEGameMessageType_Failure);
                return GetScriptAdmin().CallScriptFromScript("StartSayDoesntWork", &p_entity, &p_entity) & 0xFFFFFF00;
            }
            EffectSystem::StartEffect("eff_inv_sharpen_01", p_entity);
            if (p_entity.Inventory.GetUseType(inventoryIndex) == gEUseType_Bolt)
            {
                p_entity.Inventory.ApplyQuality(inventoryIndex, desiredQualityAddition,
                                                p_entity.Inventory.GetAmount(inventoryIndex));
                break;
            }
            p_entity.Inventory.ApplyQuality(inventoryIndex, desiredQualityAddition, 1);
            break;

        case gEItemQuality_Poisoned:
            if ((itemQuality & gEItemQuality_Poisoned) == gEItemQuality_Poisoned)
            {
                gui2.PrintGameMessage(eCLocString("HUD_AlreadyPoisoned").GetString(), gEGameMessageType_Failure);
                return GetScriptAdmin().CallScriptFromScript("StartSayDoesntWork", &p_entity, &p_entity) & 0xFFFFFF00;
            }
            stackItemIndex = p_entity.Inventory.FindStackIndex(Template("It_Potion_Poison"));
            EffectSystem::StartEffect("eff_inv_poison_01", p_entity);
            if (p_entity.Inventory.GetUseType(inventoryIndex) == gEUseType_Bolt)
            {
                GEInt amount = p_entity.Inventory.GetAmount(inventoryIndex);
                eCSceneAdmin *sceneAdmin =
                    dynamic_cast<eCSceneAdmin *>(eCModuleAdmin::GetInstance().FindModule("eCSceneAdmin"));
                auto templateEntity = sceneAdmin->GetTemplateEntityByName("It_Recipe_PoisonArrow");
                Entity recipeEntity = templateEntity;
                if (templateEntity != nullptr && recipeEntity != None)
                {
                    GEInt recipeAmount = recipeEntity.Recipe.GetProperty<PSRecipe::PropertyResultAmount>();
                    if (amount > recipeAmount)
                        amount = recipeAmount;
                }
                p_entity.Inventory.ApplyQuality(inventoryIndex, desiredQualityAddition, amount);
                break;
            }
            p_entity.Inventory.ApplyQuality(inventoryIndex, desiredQualityAddition, 1);
            break;
    }

    if (stackItemIndex > -1)
    {
        GEInt itemAmount = p_entity.Inventory.GetAmount(stackItemIndex);
        if (itemAmount < 2)
        {
            gui2.ClosePage();
        }
        p_entity.Inventory.DeleteItems(stackItemIndex, 1);
    }
    return 1;
}
GEInt GE_STDCALL OnPlayerTransformStack(gCScriptProcessingUnit *a_pSPU, Entity *a_pSelfEntity, Entity *a_pOtherEntity,
                                        GEU32 a_iArgs)
{
    INIT_SCRIPT_EXT(Self, Other);
    // print ( "a_iArgs: Int? %d, Hex? %#x\n" , a_iArgs , a_iArgs );
    gEGamePage gamePage = gui2.GetActivePage();
    gEItemQuality desiredQuality = gEItemQuality_None;
    GEInt index = *(GEInt *)a_iArgs;
    switch (gamePage)
    {
        case gEGamePage_FryMeat:
            GetScriptAdmin().CallScriptFromScript("OnPlayerFryStack", &Self, &Other, a_iArgs);
            break;
        case gEGamePage_BlessWeapon:   ModifyQuality(gEItemQuality_Blessed, index, Self); break;
        case gEGamePage_PoisonWeapon:  ModifyQuality(gEItemQuality_Poisoned, index, Self); break;
        case gEGamePage_SharpenWeapon: ModifyQuality(gEItemQuality_Sharp, index, Self); break;
    }
    Self.Inventory.ArrangeStacks();
    return 1;
}*/

static mCFunctionHook Hook_PS_Interact;
DECLARE_SCRIPT(PS_Interact)
{
    INIT_SCRIPT_EXT(Self, Other);

    Entity Target = Self.NPC.GetCurrentTarget();
    if (Target.Interaction.GetUseType() != gEUseType_Anvil)
        return Hook_PS_Interact.GetOriginalFunction(&PS_Interact)(a_pSPU, a_pSelfEntity, a_pOtherEntity, a_iArgs);

    auto EntityList = Self.GetNPCs();
    Entity CurrentEntity;
    for (GEInt i = 0; i < EntityList.GetCount(); i++)
    {
        CurrentEntity = EntityList.GetAt(i);
        if (CurrentEntity == Self)
            continue;

        if (CurrentEntity.NPC.GetProperty<PSNpc::PropertyCombatState>() == 1
            && CurrentEntity.Routine.GetProperty<PSRoutine::PropertyAIMode>() == gEAIMode_Combat
            && CurrentEntity.NPC.GetCurrentTarget() == Self)
        {
            gui2.PrintGameMessage(eCLocString("HUD_NotDuringBattle").GetString(), gEGameMessageType_Failure);
            GetScriptAdmin().CallScriptFromScript("StartSayDoesntWork", &Self, &Self);
            Self.Routine.ContinueRoutine();
            return GETrue;
        }
    }

    if (!Self.Inventory.HasItems("It_Hammer_Smith", 1))
    {
        bCUnicodeString message = eCLocString("HUD_MissingItem_Interact").GetString() + eCLocString("FO_It_Hammer_Smith").GetString();
        gui2.PrintGameMessage(message, gEGameMessageType_Failure);

        Self.Routine.ContinueRoutine();
        return GETrue;
    }
}

extern "C" __declspec(dllexport) gSScriptInit const *GE_STDCALL ScriptInit(void)
{
    // Ensure that that Script_Game.dll is loaded.
    GetScriptAdmin().LoadScriptDLL("Script_Game.dll");

    LoadSettings();

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
            ScriptInit();
            ::DisableThreadLibraryCalls(hModule);
            break;
        case DLL_PROCESS_DETACH: break;
    }
    return TRUE;
}
