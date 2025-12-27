#include "Script_UI.h"

#include "Script.h"

gSScriptInit &GetScriptInit()
{
    static gSScriptInit s_ScriptInit;
    return s_ScriptInit;
}

EXPGUI *g_EXPGUI;

/*static mCFunctionHook Hook_DoDataExchange;
void DoDataExchange ( CFFGFCDataExchange* data ) {
    std::cout << "Started Exchange!!!!\n";
    Hook_DoDataExchange.GetOriginalFunction ( &DoDataExchange )( data );

    //FF_DDX_Control ( data , IDC_STATIC_EXP , expBar );
}

static mCFunctionHook Hook_OnPageChangedSkills;
void OnPageChangedSkills ( gEHUDPage newPage , gEHUDPage oldPage ) {
    Hook_OnPageChangedSkills.GetOriginalFunction ( &OnPageChangedSkills )( newPage , oldPage );
    if ( !oldPage ) {
        expBar.SetPos ( 90 );
    }
}

static mCFunctionHook Hook_OnPageChangedInventory;
void OnPageChangedInventory ( gEHUDPage newPage , gEHUDPage oldPage ) {
    Hook_OnPageChangedInventory.GetOriginalFunction ( &OnPageChangedInventory )( newPage , oldPage );
    if ( !oldPage ) {
        expBar.SetPos ( 90 );
    }
}*/
static mCFunctionHook Hook_OnPageChangedMain;
void OnPageChangedMain(gEHUDPage newPage, gEHUDPage oldPage)
{
    Hook_OnPageChangedMain.GetOriginalFunction (&OnPageChangedMain)(newPage, oldPage);
    if (newPage != -1 && newPage != 0 && newPage != 3)
    {
        //g_EXPGUI->ShowWindow(GEFalse);
        g_EXPGUI->AnimateWindow(200, 0x90000);
    }
    else
    {
        //g_EXPGUI->ShowWindow(GETrue);
        g_EXPGUI->AnimateWindow(200, 0xA0000);
    }
}

extern "C" __declspec(dllexport) gSScriptInit const *GE_STDCALL ScriptInit(void)
{
    g_EXPGUI = GE_NEW(EXPGUI);
    CFFGFCWnd *mainDialog = (*(CFFGFCWnd **)(gCSession::GetInstance().GetGUIManager() + 1)); // Address + 0x4!
    std::cout << "mainDialog: " << mainDialog << "\n";
    CFFGFCWnd *pageMain = (*(CFFGFCWnd **)((DWORD)mainDialog + 0x7C));
    CFFGFCWnd *pageInventory = (*(CFFGFCWnd **)((DWORD)mainDialog + 0x38));
    CFFGFCWnd *pageSkills = (*(CFFGFCWnd **)((DWORD)mainDialog + 0x44));
    CFFGFCWnd *pageCharacter = (*(CFFGFCWnd **)((DWORD)pageInventory + 0x4C));

    CFFGFCWnd *viewBarHealth = (*(CFFGFCWnd **)((DWORD)pageMain + 0x60));

    /*Hook_DoDataExchange
        .Prepare ( LPVOID ( RVA_Game ( 0x7e5e0 ) ) , &DoDataExchange , mCBaseHook::mEHookType_ThisCall )
        .Hook ( );

    Hook_OnPageChangedInventory
        .Prepare ( LPVOID ( RVA_Game ( 0x87f50 ) ) , &OnPageChangedInventory , mCBaseHook::mEHookType_ThisCall )
        .Hook ( );*/

    /*Hook_OnPageChangedSkills
        .Prepare ( LPVOID ( RVA_Game ( 0x89170 ) ) , &OnPageChangedSkills , mCBaseHook::mEHookType_ThisCall )
        .Hook ( );*/

    Hook_OnPageChangedMain.Prepare(LPVOID(RVA_Game(0x88a30)), &OnPageChangedMain, mCBaseHook::mEHookType_ThisCall)
        .Hook();

    // pageCharacter->UpdateData ( GEFalse );

    bCRect clientRect;
    g_EXPGUI->GetDesktopWindow()->GetClientRect(clientRect);
    g_EXPGUI->Create(clientRect, mainDialog, 1000);
    // g_EXPGUI->InitLocation ( mainPage );
    if (!g_EXPGUI->expLocation.SubclassDlgItem(pageMain, IDC_STATIC_EXP))
    {
        std::cout << "Issue with expLocation: " << &g_EXPGUI->expLocation << "\n";
        return &GetScriptInit();
    }
    g_EXPGUI->OnInitDialog();
    g_EXPGUI->ShowWindow(GETrue);

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
    case DLL_PROCESS_DETACH: GE_DELETE(g_EXPGUI); break;
    }
    return TRUE;
}
