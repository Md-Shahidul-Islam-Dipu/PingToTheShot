// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PingToTheShot/BlasterTypes/TurningInPlace.h"
#include "BlasterAninmInstance.generated.h"

/**
 * 
 */
UCLASS()
class PINGTOTHESHOT_API UBlasterAninmInstance : public UAnimInstance
{
	GENERATED_BODY() 
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:

	UPROPERTY(BlueprintReadOnly,Category = Character,meta = (AllowPrivateAccess = "true"))
	class ABlasterCharacter* BlasterCharacter;

	UPROPERTY(BlueprintReadOnly,Category = Movement,meta = (AllowPrivateAccess = "true"))
	float Speed;
	UPROPERTY(BlueprintReadOnly,Category = Movement,meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;
	UPROPERTY(BlueprintReadOnly,Category = Movement,meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;
	UPROPERTY(BlueprintReadOnly,Category = Movement,meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched;	
	UPROPERTY(BlueprintReadOnly,Category = Movement,meta = (AllowPrivateAccess = "true"))
	bool bWeaponEquipped;	
	UPROPERTY(BlueprintReadOnly,Category = Movement,meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;	

	class AWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadOnly,Category = Movement,meta = (AllowPrivateAccess = "true"))
	float YawOffset;	
	UPROPERTY(BlueprintReadOnly,Category = Movement,meta = (AllowPrivateAccess = "true"))
	float Lean;	
	UPROPERTY(BlueprintReadOnly,Category = Movement,meta = (AllowPrivateAccess = "true"))
	float AO_Yaw;
	UPROPERTY(BlueprintReadOnly,Category = Movement,meta = (AllowPrivateAccess = "true"))
	float AO_Pitch;
	
	FRotator CharacterRotationLastFrame;
	FRotator CharacterRotation;
	FRotator DeltaRotation;

	UPROPERTY(BlueprintReadOnly,Category = Movement,meta = (AllowPrivateAccess = "true"))
	FTransform LeftHandTransform;

	UPROPERTY(BlueprintReadOnly,Category = Movement,meta = (AllowPrivateAccess = "true"))
	ETurningInPlace TurningInPlace;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	FRotator RightHandRotation;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bLocallyControlled;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bRotateRootBone;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bElimmed;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bUseFABRIK;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bUseAimOffsets;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bTransfromRightHand;
};
