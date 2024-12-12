// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PINGTOTHESHOT_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuffComponent();
	friend class ABlasterCharacter;
	void Heal(float HealAmount,float HealingTime);
	void BuffSpeed(float BuffBaseSpeed,float BuffCrouched,float BuffTime);
	void BuffJump(float BuffJumpVelocity,float BuffTime);

	void SetInitialSpeeds(float BaseSpeed,float CrouchSpeed);
	void SetInitialJumpSpeed(float Velocity);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);
private:
	UPROPERTY()
	class ABlasterCharacter* Character;

	bool bHealing = false;
	float HealingRate = 0.f;
	float AmountToHeal = 0.f;public:	

	FTimerHandle SpeedBuffTimer;
	void ResetSpeeds();
	float InitialBaseSpeed;
	float InitialCrouchSpeed;
	UFUNCTION(NetMulticast,Reliable)
	void MulticastSpeedBuff(float BaseSpeed,float CrouchSpeed);

	
	FTimerHandle JumpBuffTimer;
	void ResetJump();
	float InitialJumpVelocity;
	UFUNCTION(NetMulticast,Reliable)
	void MulticastJumpBuff(float JumpVelocity);

};