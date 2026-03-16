#include "Script_RestoreWeapons.h"
#include "RestoreGui.h"

CFFGFCWnd *g_pPageSynth;

gSScriptInit &GetScriptInit()
{
    static gSScriptInit s_ScriptInit;
    return s_ScriptInit;
}

extern "C" __declspec(dllexport) gSScriptInit const *GE_STDCALL ScriptInit(void)
{
    
    CFFGFCWnd *mainDialog = (*(CFFGFCWnd **)(gCSession::GetInstance().GetGUIManager() + 1)); // Address + 0x4!

    // Get the Handle to the Synth2 Page and to the PagesBar
    // Then add button
    // Finally link extra button to newly created intentory CFFGFCListCtrl
    // CFFGFCListCtrl should be a custom class object which overwrites the OnCommand and OnNotify functionality
    // In order to restore worn weapons!

    if (!mainDialog)
        return &GetScriptInit();

    g_pPageSynth = mainDialog->GetDlgItem(30073);
    
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
            AllocConsole();
            freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
            ::DisableThreadLibraryCalls(hModule);
            break;
        case DLL_PROCESS_DETACH: break;
    }
    return TRUE;
}
