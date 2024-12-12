// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAninmInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PingToTheShot/Weapon/Weapon.h"
#include "PingToTheShot/BlasterTypes/CombatState.h"
#include "PingToTheShot/BlasterComponents/CombatComponent.h"

void UBlasterAninmInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner()); 
}

void UBlasterAninmInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);
    if(BlasterCharacter == nullptr)
    {
        BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
    }
    if(BlasterCharacter == nullptr)return;


    FVector Velocity = BlasterCharacter->GetVelocity();
    Velocity.Z = 0.f;
    Speed = Velocity.Size();

    bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
    bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f? true : false;
    bIsCrouched = BlasterCharacter->bIsCrouched;
    bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
    EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
    bIsAiming = BlasterCharacter->IsAiming();
    TurningInPlace = BlasterCharacter->GetTurningInPlace();
    bRotateRootBone = BlasterCharacter->ShouldRotateRootBone();
    bElimmed = BlasterCharacter->IsElimmed();

    //Offset Yaw Strafing
    FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
    FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
    FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation,AimRotation);
    DeltaRotation = FMath::RInterpTo(DeltaRotation,DeltaRot,DeltaTime,6.f);
    YawOffset = DeltaRotation.Yaw;

    //Lean
    CharacterRotationLastFrame = CharacterRotation;
    CharacterRotation = BlasterCharacter->GetActorRotation();
    const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation,CharacterRotationLastFrame);
    const float Target = Delta.Yaw/DeltaTime;
    const float Interp = FMath::FInterpTo(Lean,Target,DeltaTime,6.f);
    Lean = FMath::Clamp(Interp,-90,90);

    AO_Yaw = BlasterCharacter->GetAO_Yaw();
    AO_Pitch = BlasterCharacter->GetAO_Pitch();

    if(bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh())
    {
        LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"),ERelativeTransformSpace::RTS_World);
        FVector OutPosition;
        FRotator OutRotation;
        BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"),LeftHandTransform.GetLocation(),FRotator::ZeroRotator,OutPosition,OutRotation);
        LeftHandTransform.SetLocation(OutPosition);
        LeftHandTransform.SetRotation(FQuat(OutRotation));
        if (BlasterCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
            FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
            FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation
                                                    (RightHandTransform.GetLocation(),
                                                        RightHandTransform.GetLocation() +
                                                            (RightHandTransform.GetLocation() - 
                                                                BlasterCharacter->GetHitTarget()) );
            RightHandRotation = FMath::RInterpTo(RightHandRotation,LookAtRotation,DeltaTime,30.f);
        } 
    }
    bUseFABRIK = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading;
    if(BlasterCharacter->IsLocallyControlled())
    {
        bUseFABRIK = !BlasterCharacter->IsLocallyReloading();
    }
    bUseAimOffsets = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading && !BlasterCharacter->GetDisableGameplay();
    bTransfromRightHand = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading && !BlasterCharacter->GetDisableGameplay();
}
 