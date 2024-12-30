// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "PingToTheShot/Character/BlasterCharacter.h"
#include "PingToTheShot/PlayerController/BlasterPlayerController.h"
#include "PingToTheShot/BlasterComponents/LagCompensationComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = InitialSpeed;
}

// void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
// {
// 	ABlasterCharacter* OwnerCharacter = Cast<ABlasterCharacter>(GetOwner());
// 	if (OwnerCharacter)
// 	{
// 		ABlasterPlayerController* OwnerController = Cast<ABlasterPlayerController>(OwnerCharacter->Controller);
// 		if (OwnerController)
// 		{
// 			if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind)
// 			{

// 				const float DamageToCause = Hit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;

// 				UGameplayStatics::ApplyDamage(OtherActor, DamageToCause, OwnerController, this, UDamageType::StaticClass());
// 				Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
// 				return;
// 			}
// 			ABlasterCharacter* HitCharacter = Cast<ABlasterCharacter>(OtherActor);
// 			if (bUseServerSideRewind && OwnerCharacter->GetLagCompensation() && OwnerCharacter->IsLocallyControlled() && HitCharacter)
// 			{
// 				OwnerCharacter->GetLagCompensation()->ProjectileServerScoreRequest(
// 					HitCharacter,
// 					TraceStart,
// 					InitialVelocity,
// 					OwnerController->GetServerTime() - OwnerController->SingleTripTime
// 				);
// 			}
// 		}
// 	}

// 	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
// }
void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    ABlasterCharacter* OwnerCharacter = Cast<ABlasterCharacter>(GetOwner());
    if (OwnerCharacter)
    {
        ABlasterPlayerController* OwnerController = Cast<ABlasterPlayerController>(OwnerCharacter->Controller);
        if (OwnerController)
        {
            // Server handles damage directly if SSR is disabled
            if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind)
            {
                const float DamageToCause = Hit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;
                UGameplayStatics::ApplyDamage(OtherActor, DamageToCause, OwnerController, this, UDamageType::StaticClass());
                Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
                return;
            }
            
            // Client-side hit with SSR enabled
            ABlasterCharacter* HitCharacter = Cast<ABlasterCharacter>(OtherActor);
            if (bUseServerSideRewind && !OwnerCharacter->HasAuthority() && OwnerCharacter->GetLagCompensation() && 
                OwnerCharacter->IsLocallyControlled() && HitCharacter)
            {
                OwnerCharacter->GetLagCompensation()->ProjectileServerScoreRequest(
                    HitCharacter,
                    TraceStart,
                    InitialVelocity,
                    OwnerController->GetServerTime() - OwnerController->SingleTripTime
                );
            }
            
            // Server handles damage with SSR
            if (OwnerCharacter->HasAuthority() && bUseServerSideRewind && HitCharacter)
            {
                // The damage will be applied through the ProjectileServerScoreRequest_Implementation
                Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
                return;
            }
        }
    }
    
    Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

#if WITH_EDITOR
void AProjectileBullet::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);
	FName PropertyName = Event.Property != nullptr ? Event.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AProjectileBullet, InitialSpeed))
	{
		if (ProjectileMovementComponent)
		{
			ProjectileMovementComponent->InitialSpeed = InitialSpeed;
			ProjectileMovementComponent->MaxSpeed = InitialSpeed;
		}
	}
}
#endif

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
	// FPredictProjectilePathParams PathParams;
	// PathParams.bTraceWithChannel = true;
	// PathParams.bTraceWithCollision = true;
	// PathParams.DrawDebugTime = 5.f;
	// PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	// PathParams.LaunchVelocity = GetActorForwardVector() * InitialSpeed;
	// PathParams.MaxSimTime = 4.f;
	// PathParams.ProjectileRadius = 5.f;
	// PathParams.SimFrequency = 30.f;
	// PathParams.StartLocation = GetActorLocation();
	// PathParams.TraceChannel = ECollisionChannel::ECC_Visibility;
	// PathParams.ActorsToIgnore.Add(this);
	// FPredictProjectilePathResult PathResult;
	// UGameplayStatics::PredictProjectilePath(this,PathParams,PathResult);
}
