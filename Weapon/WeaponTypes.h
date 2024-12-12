#pragma once

#define CustomDepth_Purple 250
#define CustomDepth_Blue 251
#define CustomDepth_Tan 252

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    EWT_AssaultRifle UMETA(DisplayName="Assult Rifle"),
    EWT_Pistol UMETA(DisplayName="Pistol"),
    EWT_RocketLauncher UMETA(DisplayName="Rocket Launcher"),
   
    
    EWT_MAX UMETA(DisplayName="DefaultMax")
};