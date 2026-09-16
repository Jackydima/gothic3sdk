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
        targetDirection.Normalize();

        bCVector finalDirection = targetDirection;

        // Random Deviation
        GEFloat max_fov_degree = NBConfig::ReflectFOV;
        if (Victim.IsPlayer())
        {
            max_fov_degree -= (Min(1.0f, static_cast<GEFloat>(Victim.PlayerMemory.GetDexterity() - 100)
                                             / 250.0f)     // Ranges from 100 Dex - 350 Dex Skill
                               * (max_fov_degree - 0.5f)); // Ranges from max -> 0.5f degree fov
        }

        max_fov_degree = Max(0.0f, max_fov_degree); // No negatives here!
        if (max_fov_degree > 0)
        {
            static std::default_random_engine generator;
            static auto random = std::uniform_real_distribution<GEFloat>(0.0f, 1.0f);
            const GEFloat max_fov = max_fov_degree * static_cast<GEFloat>(M_PI) / 180.0f; // Radial

            GEFloat val1 = random(generator);
            GEFloat val2 = random(generator);

            // Uniform Random Angels
            GEFloat azi = 2.0f * static_cast<GEFloat>(M_PI) * val1;
            GEFloat cos_pol = 1.0f - val2 * (1.0f - cos(max_fov));
            GEFloat sin_pol = static_cast<GEFloat>(sqrt(1.0f - pow(cos_pol, 2)));

            // Rotation Matrix with direction for z, and rotation in x,y plane
            GEFloat x_local = sin_pol * cosf(azi);
            GEFloat y_local = sin_pol * sinf(azi);
            GEFloat z_local = cos_pol;

            // Transform global calculated randomisation to local direction vector 
            bCVector forward = targetDirection;
            bCVector reference =
                (fabsf(forward.GetY()) < 0.99f) ? bCVector(0.0f, 1.0f, 0.0f) : bCVector(1.0f, 0.0f, 0.0f);
            bCVector right = bCVector::CrossProduct(reference, forward).GetNormalized();
            bCVector up = bCVector::CrossProduct(forward, right).GetNormalized();

            finalDirection = (right * x_local) + (up * y_local) + (forward * z_local);
            finalDirection.Normalize();
        }
        // Random Deviation END

        Damager.Decay();

        Spawn.Projectile.AccessProperty<PSProjectile::PropertyShootVelocity>() =
            static_cast<GEFloat>(NBConfig::shootVelocity * 0.5f);
        Spawn.Projectile.AccessProperty<PSProjectile::PropertyTargetDirection>() = finalDirection;
        Spawn.Projectile.AccessProperty<PSProjectile::PropertyPathStyle>() = gEProjectilePath_Physics;
        Spawn.CollisionShape.CreateShape(eECollisionShapeType_Point, eEShapeGroup_Projectile, bCVector(0, 0, 0),
                                         bCVector(0, 0, 0));
        Spawn.Projectile.Shoot();

        // Sound Effect
        EffectSystem::StartEffect("eff_col_wh_01_me_me", Victim);
    }

    return GETrue;
}
