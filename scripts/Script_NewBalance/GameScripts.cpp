#include "GameScripts.h"

ME_DEFINE_AND_REGISTER_SCRIPT(GetNPCBonusDamage)
{
    INIT_SCRIPT_EXT(Damager, Victim);

    Entity DamagerOwner = Damager.GetOwner();
    if (None == DamagerOwner || !DamagerOwner.Navigation.IsValid())
    {
        return 0;
    }

    GEInt iRawDamage = a_iArgs;
    GEInt iNPCLevel = GetScriptAdmin().CallScriptFromScript("GetCurrentLevel", &DamagerOwner, &None);

    if (IsSpellContainerNB(Damager) && 35 <= iNPCLevel)
    {
        return iRawDamage;
    }
    else
    {
        if (iNPCLevel >= 50)
        {
            return 50;
        }

        if (iNPCLevel >= 30)
        {
            return 25;
        }
    }

    return 0;
}
