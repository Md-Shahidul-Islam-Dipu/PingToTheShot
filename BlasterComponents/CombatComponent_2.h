// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PingToTheShot/HUD/BlasterHUD.h"
#include "Components/ActorComponent.h"
#include "PingToTheShot/Weapon/WeaponTypes.h"
#include "PingToTheShot/BlasterTypes/CombatState.h"
#include "CombatComponent_2.generated.h"


#define TRACE_LENGTH  80000.f;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PINGTOTHESHOT_API UCombatComponent_2 : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCombatComponent_2();
	friend class ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void EquipWeapon(class AWeapon* WeaponToEquip);
	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

    void FireButtonPressed(bool bPressed);

    void Fire();

    UFUNCTION(Server,Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast,Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server,Reliable)
	void ServerReload();

	void HandleReload();

	int32 AmountToReload();
private:
	UPROPERTY()
	class ABlasterCharacter* Character;
	UPROPERTY()
	class ABlasterPlayerController* Controller;
	UPROPERTY()
	class ABlasterHUD* HUD;

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	FHUDPackage HUDPackage;

	FVector HitTarget;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;	
	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	float DefaultFOV;
	UPROPERTY(EditAnywhere,Category = Combat)
	float ZoomedFOV = 30.f;
	UPROPERTY(EditAnywhere,Category = Combat)
	float ZoomInterpSpeed= 20.f;
	void InterpFOV(float DeltaTime);
	float CurrentFOV;

	// FTimerHandle FireTimer;
	// bool bCanFire = true;
	// void StartFireTimer();
	// void FireTimerFinished();

	bool CanFire();

	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;
	UFUNCTION()
	void OnRep_CarriedAmmo();
	TMap<EWeaponType,int32> CarriedAmmoMap;
	void InitializeCarriedAmmo();

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
};

