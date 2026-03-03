#include "ge_DamageReceiver_ext.h"

#include "me_propertymacros.h"

ME_DEFINE_PROPERTY_TYPE(gCDamageReceiver_PS_Ext, gCDamageReceiver_PS, PoisonDamage, m_PoisonDamage)
ME_DEFINE_PROPERTY_TYPE(gCDamageReceiver_PS_Ext, gCDamageReceiver_PS, VulnerableState, m_VulnerableState)

gCDamageReceiver_PS_Ext::gCDamageReceiver_PS_Ext(void) : m_PoisonDamage(0), m_VulnerableState(0)
{}

void gCDamageReceiver_PS_Ext::Invalidate()
{
    gCDamageReceiver_PS::Invalidate();
    this->m_PoisonDamage = 0;
    this->m_VulnerableState = 0;
}

ME_MODULE(gCDamageReceiver_PS_Ext)
{
    // Extended Makro with adjustments to the hooking logic:
    // Compiler optimization made the type of the pushed size of class to 1 Byte because of the small class size
    // Second the gCDamageReceiver_PS::Invalidate() function in the Game.dll is just one byte length (ret)
    __pragma(warning(push)) __pragma(warning(disable : 4127)) do
    {
        static mCDataPatch Patch_CreatePropertySetSize(RVA_Game(0x3b96C),
                                                       static_cast<BYTE>(sizeof(gCDamageReceiver_PS_Ext)));
        static mCFunctionHook Hook_Constructor;
        Hook_Constructor
            .Prepare(GetProcAddress("Game.dll", "??0" + bTClassName<gCDamageReceiver_PS>::GetUnmangled() + "@@QAE@XZ"),
                     GetProcAddress("Script_NewBalance.dll",
                                    "??0" + bTClassName<gCDamageReceiver_PS_Ext>::GetUnmangled() + "@@QAE@XZ"),
                     mCBaseHook::mEHookType_ThisCall)
            .Transparent()
            .Hook();
        static mCFunctionHook Hook_Destructor;
        Hook_Destructor
            .Prepare(GetProcAddress("Game.dll", "??1" + bTClassName<gCDamageReceiver_PS>::GetUnmangled() + "@@UAE@XZ"),
                     GetProcAddress("Script_NewBalance.dll",
                                    "??1" + bTClassName<gCDamageReceiver_PS_Ext>::GetUnmangled() + "@@UAE@XZ"),
                     mCBaseHook::mEHookType_ThisCall)
            .Transparent()
            .Hook();
    }
    while (0)
        ;
    __pragma(warning(pop))
}
