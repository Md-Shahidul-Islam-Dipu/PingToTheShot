// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PingToTheShot/BlasterTypes/TurningInPlace.h"
#include "PingToTheShot/Interfaces/InteractWithCrosshairsInterface.h"
#include "PingToTheShot/BlasterTypes/CombatState.h"
#include "Components/TimelineComponent.h"
#include "BlasterCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class PINGTOTHESHOT_API ABlasterCharacter : public ACharacter ,public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlasterCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void Tick(float DeltaTime) override;
   
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	void PlayHitReactMontage();
	virtual void OnRep_ReplicatedMovement() override;
    void DropOrDestroyWeapons();
	void Elim(bool bPlayerLeftGame);
    UFUNCTION(NetMulticast,Reliable)
	void MulticastElim(bool bPlayerLeftGame);
	virtual void Destroyed() override;
	UPROPERTY(Replicated)
	bool bDisableGameplay = false;
	void UpdateHUDHealth();
	void UpdateHUDAmmo();
	UFUNCTION(Server,Reliable)
	void ServerLeaveGame();
	FOnLeftGame OnLeftGame;

	UFUNCTION(NetMulticast,Reliable)
	void MulticastGainedTheLead();
	UFUNCTION(NetMulticast,Reliable)
	void MulticastLostTheLead();

protected:
    virtual void BeginPlay() override;
    void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void CrouchButtonPressed();	
	void EquipButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	void ReloadButtonPressed();
	virtual void Jump() override;
    void AimOffset(float DeltaTime);
    void CalculatedAO_Pitch();
    void SimProxiesTurn();
	void RotateInPlace(float DeltaTime);
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	//polling for relevent classes and initializing HUD in the first frame
	void PollInit();
	void SpawnDefaultWeapon();
	void DropOrDestroyWeapon(AWeapon* Weapon);
private:
	UPROPERTY(VisibleAnywhere,Category="Components")
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere,Category="Components")
	class UCameraComponent* CameraComp;
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* CombatComponent;
	UPROPERTY(VisibleAnywhere)
	class UBuffComponent* BuffComponent;
	UFUNCTION(Server,Reliable)
	void ServerEquipButtonPressed();

	UPROPERTY(EditAnywhere,Category = Combat)
	class UAnimMontage* FireWeaponMontage;
	UPROPERTY(EditAnywhere,Category = Combat)
	UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere,Category = Combat)
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere,Category = Combat)
	UAnimMontage* ElimMontage;

	float AO_Yaw;
	float InterpedAo_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	void HideCharacterIfCameraClose();
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplicated;

	float CalculatedSpeed();

	UPROPERTY(EditAnywhere,Category = "Player Stat")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health,VisibleAnywhere,Category = "Player Stat")
	float Health = 100.f;
	UFUNCTION()
	void OnRep_Health(float LastHealth);

	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController;

	bool bElimmed;
	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
	void ElimTimerFinished();
	bool bLeftGame = false;

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrack;
	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();
	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;
	UPROPERTY(VisibleAnywhere,Category = Elim)
	UMaterialInstanceDynamic*  DynamicDissolveMaterialInstance;
	UPROPERTY(EditAnywhere,Category = Elim)
	UMaterialInstance* DissolveMaterialInstance;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;
	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound;
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* CrownSystem;
	UPROPERTY()
	class UNiagaraComponent* CrownComponent;

	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;
public:	
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	AWeapon* GetEquippedWeapon();
	FVector GetHitTarget() const;
	FORCEINLINE float GetAO_Yaw() const {return AO_Yaw;}
	FORCEINLINE float GetAO_Pitch() const {return AO_Pitch;}
	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return TurningInPlace;}
	FORCEINLINE UCameraComponent* GetCameraComp() const {return CameraComp;}
	FORCEINLINE bool ShouldRotateRootBone() const {return bRotateRootBone;}
	FORCEINLINE bool IsElimmed() const {return bElimmed;}
	FORCEINLINE float GetHealth() const {return Health;}
	FORCEINLINE void SetHealth(float Amount){Health = Amount;}
	FORCEINLINE float GetMaxHealth() const{return MaxHealth;}
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const {return CombatComponent;}
	FORCEINLINE UBuffComponent* GetBuff() const {return BuffComponent;}
	FORCEINLINE bool GetDisableGameplay() const {return bDisableGameplay;}
	bool IsLocallyReloading();
};
