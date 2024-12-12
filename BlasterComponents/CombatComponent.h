// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PingToTheShot/HUD/BlasterHUD.h"
#include "Components/ActorComponent.h"
#include "PingToTheShot/Weapon/WeaponTypes.h"
#include "PingToTheShot/BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH  80000.f;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PINGTOTHESHOT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCombatComponent();
	friend class ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void EquipWeapon(class AWeapon *WeaponToEquip);
	void SwapWeapons();
    void ReloadEmptyWeapon();
    void PlayEquipWeaponSound(AWeapon* WeaponToEquip);
    void UpdateCarriedAmmo();
    void DropEquippedWeapon();
    void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	void FireButtonPressed(bool bPressed);
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);
	void PickupAmmo(EWeaponType WeaponType,int32 AmmoAmount);

	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToBackpack(AActor* ActorToAttach);

	bool bLocallyReloading =false;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
	UFUNCTION()
	void OnRep_SecondaryWeapon();
	
    void Fire();
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);

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
	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	AWeapon* SecondaryWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming = false;

	bool bAimButtonPressed = false;

	UFUNCTION()
	void OnRep_Aiming();
	
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
	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 500;
	void InitializeCarriedAmmo();

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;
	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 15;
	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 0;
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();

public:
	bool ShouldSwapWeapons();
};
