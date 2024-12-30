// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "PingToTheShot/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PingToTheShot/BlasterComponents/LagCompensationComponent.h"
#include "PingToTheShot/PlayerController/BlasterPlayerController.h"

void AHitScanWeapon::Fire(const FVector &HitTarget)
{
    Super::Fire(HitTarget);

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if(OwnerPawn == nullptr) return;
    AController* InstigatorController = OwnerPawn->GetController();
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
    if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        FVector Start = SocketTransform.GetLocation();
        FVector End = Start + (HitTarget - Start) * 1.25f;

        FHitResult FireHit;
        UWorld* World = GetWorld();
        if(World)
        {
            World->LineTraceSingleByChannel(
                FireHit,
                Start,
                End,
                ECollisionChannel::ECC_Visibility
            );
            FVector BeamEnd = End;
            if(FireHit.bBlockingHit)
            {
                BeamEnd = FireHit.ImpactPoint;
                ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
                if(BlasterCharacter && InstigatorController)
                {
                    // bool bCauseAuthDamage = OwnerPawn->HasAuthority() && !bUseServerSideRewind;
                    // if(HasAuthority() && bCauseAuthDamage)
                    // {
                    //     const float DamageToCause = FireHit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;
                    //     UGameplayStatics::ApplyDamage(
                    //         BlasterCharacter,
                    //         DamageToCause,
                    //         InstigatorController,
                    //         this,
                    //         UDamageType::StaticClass()
                    //     );
                    // }
                    if(HasAuthority())
                    {
                        bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
                        if(bCauseAuthDamage)
                        {
                            const float DamageToCause = FireHit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;
                            UGameplayStatics::ApplyDamage(
                                BlasterCharacter,
                                DamageToCause,
                                InstigatorController,
                                this,
                                UDamageType::StaticClass()
                            );
                        }
                    }
                    if (!HasAuthority() && bUseServerSideRewind)
                    {
                        BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
				        BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
                        if (BlasterOwnerController && BlasterOwnerCharacter && BlasterOwnerCharacter->GetLagCompensation())
				        {
					        BlasterOwnerCharacter->GetLagCompensation()->ServerScoreRequest(
						        BlasterCharacter,
						        Start,
						        HitTarget,
						        BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime
					        );
				        }
                    }  
                }
                if(ImpactParticles)
                {
                    UGameplayStatics::SpawnEmitterAtLocation(
                        World,
                        ImpactParticles,
                        FireHit.ImpactPoint,
                        FireHit.ImpactNormal.Rotation()
                    );
                }
            }
            else
            {
                FireHit.ImpactPoint = End;
            }
            if(BeamParticles)
            {
                UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
                    World,
                    BeamParticles,
                    SocketTransform
                );
                if(Beam)
                {
                    Beam->SetVectorParameter(FName("Target"),BeamEnd);
                }
            }
        }
    }
}
