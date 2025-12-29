
#include "utils.h"

DWORD CallWithEAX(DWORD value, DWORD adr)
{
    DWORD result;

    __asm {
        mov eax, value
        call adr
        mov result, eax // capture return value
    }

    return result;
}
