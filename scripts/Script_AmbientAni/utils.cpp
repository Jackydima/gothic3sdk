
#include "utils.h"

/*DWORD CallWithEAX(DWORD value, DWORD adr)
{
    DWORD result;

    __asm {
        mov eax, value
        call adr
        mov result, eax // capture return value
    }

    return result;
}*/

using FuncTypeEaxEax = DWORD (__fastcall *)(void);
DWORD CallWithEAX(DWORD value, DWORD adr)
{
    static mCCaller CallWithEaxRetEax(mCCaller::GetCallerParams(adr, mERegisterType_Eax));
    CallWithEaxRetEax.SetImmEax(value);
    return CallWithEaxRetEax.GetFunction<FuncTypeEaxEax>()();
}
