#include "Script_AdjustedTheft.h"

gSScriptInit &GetScriptInit()
{
    static gSScriptInit s_ScriptInit;
    return s_ScriptInit;
}

static mCFunctionHook Hook_IsBotheredByPlayerTheftOf;

// Self: Current Entity, Other: Taken item
// Other perception logic is already take care of in AssessItemTaken
DECLARE_SCRIPT(IsBotheredByPlayerTheftOf)
{
    INIT_SCRIPT_EXT(Self, Other);
    if (/* !Other.Item.IsValid() ||*/ !Other.Interaction.IsValid())
        // Should not happen? Maybe Chests then
        return Hook_IsBotheredByPlayerTheftOf.GetOriginalFunction(&IsBotheredByPlayerTheftOf)(a_pSPU, a_pSelfEntity,
                                                                                              a_pOtherEntity, a_iArgs);
    Entity owner = Other.Interaction.GetOwner();
#ifndef NDEBUG
    std::cout << "First Self!: " << Self.GetName() << "\n";
    std::cout << "First Owner!: " << owner.GetName() << "\n";
#endif

    if (owner == None)
        return Hook_IsBotheredByPlayerTheftOf.GetOriginalFunction(&IsBotheredByPlayerTheftOf)(a_pSPU, a_pSelfEntity,
                                                                                              a_pOtherEntity, a_iArgs);
    // If NPC is close friend
    if (Self.NPC.Type == gENPCType_Friend) // Close friend type
    {
        return Hook_IsBotheredByPlayerTheftOf.GetOriginalFunction(&IsBotheredByPlayerTheftOf)(a_pSPU, a_pSelfEntity,
                                                                                              a_pOtherEntity, a_iArgs);
    }

    // NPC as Owner
    if (owner.Navigation.IsValid())
    {
        if (owner.NPC.Type == gENPCType_Friend) // Close friend type
        {
            return Hook_IsBotheredByPlayerTheftOf.GetOriginalFunction(&IsBotheredByPlayerTheftOf)(
                a_pSPU, a_pSelfEntity, a_pOtherEntity, a_iArgs);
        }

        // Owners are getting mad at player
        if (Self == owner)
        {
            return GETrue;
        }

        // If NPCs in Range are befriended with the Owner they do too
        gEAttitude att = static_cast<gEAttitude>(RUN_SCRIPT("GetAttitude", a_pSelfEntity, &owner, 0));
#ifndef NDEBUG
        std::cout << "Friends?: " << att << "\n"; 
#endif
        if (att == gEAttitude_Friendly)
            return GETrue;

        return Hook_IsBotheredByPlayerTheftOf.GetOriginalFunction(&IsBotheredByPlayerTheftOf)(a_pSPU, a_pSelfEntity,
                                                                                              a_pOtherEntity, a_iArgs);
    }

    // Enclave?!
    if (Self.NPC.GetEnclave() == owner)
    {
#ifndef NDEBUG
        std::cout << "Enclave!: " << Self.NPC.GetEnclave().GetName() << "\n";
        std::cout << "Other!: " << owner.GetName() << "\n";
#endif
        if (gEAttitude_Friendly == static_cast<gEAttitude>(RUN_SCRIPT("GetPoliticalAttitude", a_pSelfEntity, &owner, 0)))
        {
            return GETrue;
        }
    }

    return Hook_IsBotheredByPlayerTheftOf.GetOriginalFunction(&IsBotheredByPlayerTheftOf)(a_pSPU, a_pSelfEntity,
                                                                                          a_pOtherEntity, a_iArgs);
}

extern "C" __declspec(dllexport) gSScriptInit const *GE_STDCALL ScriptInit(void)
{
    GetScriptAdminExt().LoadScriptDLL("Script_Game.dll");
    Hook_IsBotheredByPlayerTheftOf.Hook(GetScriptAdminExt().GetScript("IsBotheredByPlayerTheftOf")->m_funcScript,
                                        &IsBotheredByPlayerTheftOf);
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
#ifndef NDEBUG
        AllocConsole();
        freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
#endif // !NDEBUG
        ::DisableThreadLibraryCalls(hModule);
        break;
    case DLL_PROCESS_DETACH: break;
    }
    return TRUE;
}
