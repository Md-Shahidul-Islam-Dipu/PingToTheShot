// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "PingToTheShot/Weapon/Weapon.h"
#include "PingToTheShot/BlasterComponents/CombatComponent.h"
#include "PingToTheShot/BlasterComponents/BuffComponent.h"
#include "PingToTheShot/BlasterComponents/LagCompensationComponent.h"
#include "BlasterAninmInstance.h"
#include "Kismet/KismetMathLibrary.h" 
#include "PingToTheShot/PingToTheShot.h"
#include "PingToTheShot/PlayerController/BlasterPlayerController.h"
#include "PingToTheShot/GameMode/BlasterGameMode.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "PingToTheShot/PlayerState/BlasterPlayerState.h"
#include "PingToTheShot/Weapon/WeaponTypes.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PingToTheShot/GameState/BlasterGameState.h"
#include "Components/BoxComponent.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
    SpringArm->SetupAttachment(GetMesh());
	SpringArm->bUsePawnControlRotation = false;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArm,USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera,ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera,ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECollisionResponse::ECR_Block);
	
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	CombatComponent->SetIsReplicated(true);

	BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("Buff"));
	BuffComponent->SetIsReplicated(true);

	LagCompensationComponent = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Dissolve Timeline Component"));

	/** 
		Hit boxes used for server-side rewind and for headshots
	*/

	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(),FName("head"));
	head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("head"), head);
	
	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	pelvis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	spine_02->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	spine_03->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	upperarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	upperarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	lowerarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	lowerarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	hand_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	hand_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);

	blanket = CreateDefaultSubobject<UBoxComponent>(TEXT("blanket"));
	blanket->SetupAttachment(GetMesh(), FName("backpack"));
	blanket->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("blanket"), blanket);

	backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
	backpack->SetupAttachment(GetMesh(), FName("backpack"));
	backpack->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("backpack"), backpack);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	thigh_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	thigh_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	calf_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	calf_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	foot_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	foot_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultWeapon();
	UpdateHUDAmmo();
    UpdateHUDHealth();
    if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this,&ABlasterCharacter::ReceiveDamage);
	}
}

void ABlasterCharacter::UpdateHUDHealth()
{
    BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
    if (BlasterPlayerController)
    {
        BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
    }
}

void ABlasterCharacter::UpdateHUDAmmo()
{
	 BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
    if (BlasterPlayerController && CombatComponent && CombatComponent->EquippedWeapon)
    {	
        BlasterPlayerController->SetHUDCarriedAmmo(CombatComponent->CarriedAmmo);
		BlasterPlayerController->SetHUDWeaponAmmo(CombatComponent->EquippedWeapon->GetAmmo());
    }
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    RotateInPlace(DeltaTime);
    HideCharacterIfCameraClose();
	PollInit();
}

void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if(bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
    if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
    {
        AimOffset(DeltaTime);
    }
    else
    {
        TimeSinceLastMovementReplicated += DeltaTime;
        if (TimeSinceLastMovementReplicated > 0.25f)
        {
            OnRep_ReplicatedMovement();
        }
        CalculatedAO_Pitch();
    }
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("MoveForward",this,&ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn",this,&ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp",this,&ABlasterCharacter::LookUp); 
	
	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ABlasterCharacter::Jump);
	PlayerInputComponent->BindAction("Crouch",IE_Pressed,this,&ABlasterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Equip",IE_Pressed,this,&ABlasterCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Aiming",IE_Pressed,this,&ABlasterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aiming",IE_Released,this,&ABlasterCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Fire",IE_Pressed,this,&ABlasterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire",IE_Released,this,&ABlasterCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction("Reload",IE_Pressed,this,&ABlasterCharacter::ReloadButtonPressed);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(CombatComponent)
	{
		CombatComponent->Character = this;
	}
	if(BuffComponent)
	{
		BuffComponent->Character = this;
		BuffComponent->SetInitialSpeeds(GetCharacterMovement()->MaxWalkSpeed,GetCharacterMovement()->MaxWalkSpeedCrouched);
		BuffComponent->SetInitialJumpSpeed(GetCharacterMovement()->JumpZVelocity);
	}
	if(LagCompensationComponent)
	{
		LagCompensationComponent->Character = this;
		if(Controller)
		{
			LagCompensationComponent->Controller = Cast<ABlasterPlayerController>(Controller);
		}
	}
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if(CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming? FName("Rifle_Aim") : FName("Rifle_Hip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayReloadMontage()
{
	if (CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		switch (CombatComponent->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("Rocket");
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName);
	}	
}

void ABlasterCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ABlasterCharacter::PlayHitReactMontage()
{
	if (CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Playing Hit React Montage"));
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("From_Left");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::ReceiveDamage(AActor *DamagedActor, float Damage, const UDamageType *DamageType, AController *InstigatorController, AActor *DamageCauser)
{	
	Health = FMath::Clamp(Health - Damage,0.f,MaxHealth);
	PlayHitReactMontage(); 
	UpdateHUDHealth();
	if (Health == 0.f)
	{
		ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
		if (BlasterGameMode)
		{
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
			ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatorController);
			BlasterGameMode->PlayerEliminated(this, BlasterPlayerController, AttackerController);
		}
	}
}
void ABlasterCharacter::PollInit()
{
	if(BlasterPlayerState == nullptr)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if(BlasterPlayerState)
		{
			BlasterPlayerState->AddToScore(0.f);
			BlasterPlayerState->AddToDefeats(0);
			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));

			if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(BlasterPlayerState))
			{
				MulticastGainedTheLead();
			}
		}
	}
}

void ABlasterCharacter::SpawnDefaultWeapon()
{
	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if (BlasterGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (CombatComponent)
		{
			CombatComponent->EquipWeapon(StartingWeapon);
		}
	}
}

void ABlasterCharacter::MoveForward(float Value)
{
	if(bDisableGameplay)return;
	if(Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f,Controller->GetControlRotation().Yaw,0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction,Value);
	}
}

void ABlasterCharacter::MoveRight(float Value)
{
	if(bDisableGameplay)return;
	if(Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f,Controller->GetControlRotation().Yaw,0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction,Value);
	}
}

void ABlasterCharacter::Turn(float Value)
{
	
	AddControllerYawInput(Value);
}

void ABlasterCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if(bDisableGameplay)return;
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ABlasterCharacter::Jump()
{
	if(bDisableGameplay)return;
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ABlasterCharacter::EquipButtonPressed()
{
	if(bDisableGameplay)return;
	if(CombatComponent)
	{	
		ServerEquipButtonPressed();
	}
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if(CombatComponent)
	{
		if(OverlappingWeapon)
		{
			CombatComponent->EquipWeapon(OverlappingWeapon);
		}
		else if(CombatComponent->ShouldSwapWeapons())
		{
			CombatComponent->SwapWeapons();
		}
	}
}

void ABlasterCharacter::AimButtonPressed()
{
	if(bDisableGameplay)return;
	if(CombatComponent)
	{
		CombatComponent->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if(bDisableGameplay)return;
	if(CombatComponent)
	{
		CombatComponent->SetAiming(false);
	}
}

void ABlasterCharacter::FireButtonPressed()
{
	if(bDisableGameplay)return;
	if(CombatComponent)
	{
		CombatComponent->FireButtonPressed(true);
	}
}

void ABlasterCharacter::FireButtonReleased()
{
	if(bDisableGameplay)return;
	if(CombatComponent)
	{
		CombatComponent->FireButtonPressed(false);
	}
}

void ABlasterCharacter::ReloadButtonPressed()
{
	if(bDisableGameplay)return;
	if(CombatComponent)
	{
		CombatComponent->Reload();
	}
}

void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if(DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"),DissolveValue);
	}

}

void ABlasterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this,&ABlasterCharacter::UpdateDissolveMaterial);
	if(DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve,DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon *Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
    return (CombatComponent && CombatComponent->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (CombatComponent && CombatComponent->bAiming);
}

AWeapon *ABlasterCharacter::GetEquippedWeapon()
{
	if(CombatComponent == nullptr) return nullptr;
	return CombatComponent->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
    if (CombatComponent == nullptr) return FVector();
	return CombatComponent->HitTarget;
}

ECombatState ABlasterCharacter::GetCombatState() const
{
	if (CombatComponent == nullptr) return ECombatState::ECS_MAX;
	return CombatComponent->CombatState;
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon *LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

float ABlasterCharacter::CalculatedSpeed()
{
	FVector Velocity = GetVelocity();
    Velocity.Z = 0.f;
    return Velocity.Size();
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if(CombatComponent && CombatComponent->EquippedWeapon == nullptr) return;
    float Speed = CalculatedSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if(Speed == 0.0 && !bIsInAir)
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation,StartingAimRotation );
		AO_Yaw = DeltaAimRotation.Yaw;
		if(TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpedAo_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if(Speed > 0.0 || bIsInAir)
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
    CalculatedAO_Pitch();
}

void ABlasterCharacter::CalculatedAO_Pitch()
{
    AO_Pitch = GetBaseAimRotation().Pitch;
    if (AO_Pitch > 90.f && !IsLocallyControlled())
    {
        FVector2D InRange(270.f, 360.f);
        FVector2D OutRange(-90.f, 0.f);
        AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
    }
}

void ABlasterCharacter::SimProxiesTurn()
{
	if(CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr)return; 

	bRotateRootBone = false;
	float Speed = CalculatedSpeed();
	if(Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation,ProxyRotationLastFrame).Yaw;
	if(FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if(ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if(ProxyYaw < - TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if(TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpedAo_Yaw = FMath::FInterpTo(InterpedAo_Yaw,0.f,DeltaTime,4.f);
		AO_Yaw = InterpedAo_Yaw;
		if(FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		}
	}
}

void ABlasterCharacter::HideCharacterIfCameraClose()
{
	if(!IsLocallyControlled())return;
	if((CameraComp->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if(CombatComponent && CombatComponent->EquippedWeapon && CombatComponent->EquippedWeapon->GetWeaponMesh())
		{
			CombatComponent->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else 
	{
		GetMesh()->SetVisibility(true);
		if(CombatComponent && CombatComponent->EquippedWeapon && CombatComponent->EquippedWeapon->GetWeaponMesh())
		{
			CombatComponent->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplicated = 0.f;
}

void ABlasterCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth(); 
	if(Health < LastHealth)
	{
		PlayHitReactMontage();
	}
}

void ABlasterCharacter::MulticastElim_Implementation(bool bPlayerLeftGame)
{
	if(BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
	}
	bElimmed = true;
	PlayElimMontage();
	if(DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance,this);
		GetMesh()->SetMaterial(0,DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"),0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"),200.f);
	}
	StartDissolve();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	bDisableGameplay = true;
	if(CombatComponent)
	{
		CombatComponent->FireButtonPressed(false);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if(ElimBotEffect)
	{
		FVector ElimBotSpawnPoint(GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z + 200.f);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
			ElimBotEffect,
			ElimBotSpawnPoint,
			GetActorRotation());
	}
	if(ElimBotSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ElimBotSound,
			GetActorLocation()
		);
	}
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&ABlasterCharacter::ElimTimerFinished,
		ElimDelay
	);
}
void ABlasterCharacter::Elim(bool bPlayerLeftGame)
{
	bLeftGame = bPlayerLeftGame;
    DropOrDestroyWeapons();
    MulticastElim(bPlayerLeftGame);
}

void ABlasterCharacter::DropOrDestroyWeapons()
{
    if (CombatComponent)
    {
        if (CombatComponent->EquippedWeapon)
        {
            DropOrDestroyWeapon(CombatComponent->EquippedWeapon);
        }
        if (CombatComponent->SecondaryWeapon)
        {
            DropOrDestroyWeapon(CombatComponent->SecondaryWeapon);
        }
    }
}

void ABlasterCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;
	if (Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{
		Weapon->Dropped();
	}
}

void ABlasterCharacter::ElimTimerFinished()
{
	ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (BlasterGameMode && !bLeftGame)
	{
		BlasterGameMode->RequestRespawn(this, Controller);
	}
	if (bLeftGame && IsLocallyControlled())
	{
		OnLeftGame.Broadcast();
	}
}

void ABlasterCharacter::ServerLeaveGame_Implementation()
{
	ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	BlasterPlayerState = BlasterPlayerState == nullptr ? GetPlayerState<ABlasterPlayerState>() : BlasterPlayerState;
	if (BlasterGameMode && BlasterPlayerState)
	{
		BlasterGameMode->PlayerLeftGame(BlasterPlayerState);
	}
}
void ABlasterCharacter::Destroyed()
{ 
	Super::Destroyed();

	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}
	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = BlasterGameMode && BlasterGameMode->GetMatchState() != MatchState::InProgress;
	if(CombatComponent && CombatComponent->EquippedWeapon && bMatchNotInProgress)
	{
		CombatComponent->EquippedWeapon->Destroy();
	}
}

bool ABlasterCharacter::IsLocallyReloading()
{
	if(CombatComponent == nullptr) return false;
	return CombatComponent->bLocallyReloading;
}

void ABlasterCharacter::MulticastGainedTheLead_Implementation()
{
	if(CrownSystem == nullptr)return;
	if(CrownComponent == nullptr)
	{
		CrownComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			CrownSystem,
			GetCapsuleComponent(),
			FName(),
			GetActorLocation() + FVector(0.f, 0.f, 110.f),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	if(CrownComponent)
	{
		CrownComponent->Activate();
	}
}

void ABlasterCharacter::MulticastLostTheLead_Implementation()
{
	if(CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter,Health);
	DOREPLIFETIME(ABlasterCharacter,bDisableGameplay);
}

