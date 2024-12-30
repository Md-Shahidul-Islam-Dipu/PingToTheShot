// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName="Initial State"),
	EWS_Equipped UMETA(DisplayName="Equipped State"),
	EWS_EquippedSecondary UMETA(DisplayName="Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName="Dropped State"),

	EWS_MAX UMETA(DisplayName="DefaultMax")
};

UCLASS()
class PINGTOTHESHOT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void OnRep_Owner() override;

    void SetHUDAmmo();
	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;

	void EnableCustomDepth(bool bEnable);

	bool bDestroyWeapon = false;
protected:

	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

    virtual void OnWeaponStateSet();
    virtual void OnDropped();
    virtual void OnEquipped();
	virtual void OnEquippedSecondary();

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 40.f;
	UPROPERTY(Replicated,EditAnywhere)
	bool bUseServerSideRewind = false;

	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;
	UPROPERTY()
	class ABlasterPlayerController* BlasterOwnerController;

	UFUNCTION()
	void OnPingTooHigh(bool bPingTooHigh);
private:
	UPROPERTY(VisibleAnywhere,Category = "Weapon Properties")
	USkeletalMeshComponent*  WeaponMesh;

	UPROPERTY(VisibleAnywhere,Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState,VisibleAnywhere,Category = "Weapon Properties")
	EWeaponState WeaponState;
	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(EditAnywhere,Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere,Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;

	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	UPROPERTY(EditAnywhere)
	int32 Ammo;

	UFUNCTION(Client,Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);
	UFUNCTION(Client,Reliable)
	void ClientAddAmmo(int32 AmmoToAdd);

	int32 Sequence = 0;

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;
	void SpendRound();

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
public:	
	void ShowPickupWidget(bool bShowWidget);	
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere ;}
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const {return WeaponMesh;}
	FORCEINLINE float GetZoomedFOV() const {return ZoomedFOV;}
	FORCEINLINE float GetZoomInterpSpeed() const {return ZoomInterpSpeed;}
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType;}
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }
	
	virtual void Fire(const FVector& HitTarget);
	
	UPROPERTY(EditAnywhere,Category = Crosshair)
	class UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere,Category = Crosshair)
	UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere,Category = Crosshair)
	UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere,Category = Crosshair)
	UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere,Category = Crosshair)
	UTexture2D* CrosshairsBottom;

	UPROPERTY(EditAnywhere,Category = Combat)
	float FireDelay = 0.15f;
	UPROPERTY(EditAnywhere,Category = Combat)
	bool bAutomatic = true;

	void Dropped();
	void AddAmmo(int32 AmmoToAdd);

	bool IsEmpty();
	
};
