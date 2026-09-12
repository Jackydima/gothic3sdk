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

ME_DEFINE_AND_REGISTER_SCRIPT(ReflectProjectile)
{
    INIT_SCRIPT_EXT(Damager, Victim);
    UNREFERENCED_PARAMETER(a_iArgs);

    gCProjectile_PS *Projectile_PS =
            GetPropertySet<gCProjectile_PS>(Damager.GetGameEntity(), eEPropertySetType_Projectile);
    if (Projectile_PS)
    {
        Template templateSpawn = Template(Damager.GetTemplate());
        bCMatrix DamagerPose = Damager.GetPose().GetRotatedByLocalAxisY(180.0f);
        Entity Spawn = Entity::Spawn(templateSpawn, DamagerPose);
        Spawn.EnableCollisionWith(Victim, GEFalse);
        Spawn.EnableCollisionWith(Spawn, GEFalse);
        Spawn.EnableCollisionWith(Damager, GEFalse);
        Spawn.Interaction.SetOwner(Victim);
        GEInt damageAmount = Damager.Damage.GetProperty<PSDamage::PropertyDamageAmount>();
        Spawn.Damage.AccessProperty<PSDamage::PropertyDamageAmount>() = damageAmount;

        bCVector targetDirection = Projectile_PS->GetTargetDirection();
        targetDirection.Scale(-1);

        Damager.Decay();
        
        Spawn.Projectile.AccessProperty<PSProjectile::PropertyShootVelocity>() =
            static_cast<GEFloat>(NBConfig::shootVelocity * 0.5f);
        Spawn.Projectile.AccessProperty<PSProjectile::PropertyTargetDirection>() = targetDirection;
        Spawn.Projectile.AccessProperty<PSProjectile::PropertyPathStyle>() = gEProjectilePath_Physics;
        Spawn.CollisionShape.CreateShape(eECollisionShapeType_Point, eEShapeGroup_Projectile, bCVector(0, 0, 0),
                                            bCVector(0, 0, 0));
        Spawn.Projectile.Shoot();
    }

    return GETrue;
}

