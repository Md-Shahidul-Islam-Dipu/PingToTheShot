// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);
/**
 * 
 */
UCLASS()
class PINGTOTHESHOT_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:	
	void SetHUDHealth(float Health,float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountDown(float CountDownTime);
	void SetHUDAnnouncementCountDown(float CountDownTime);
    virtual void Tick(float DeltaTime) override;
    void CheckTimeSync(float DeltaTime);
    virtual void OnPossess(APawn* InPawn) override;
	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;
	void OnMatchStateSet(FName State);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void HandleMatchHasStarted();
	void HandleCooldown();
	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim);
	float SingleTripTime = 0.f;
	FHighPingDelegate HighPingDelegate;
protected:
	virtual void BeginPlay() override;
	void SetHUDTime();
	void PollInit();
	// Requests the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
 
	// Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f;
	UPROPERTY(EditAnywhere,Category = Time)
	float TimeSyncFrequency = 5.f;
	float TimeSyncRunningTime = 0.f;

	UFUNCTION(Server,Reliable)
	void ServerCheckMatchState();
	UFUNCTION(Client,Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match,float Cooldown, float StartingTime);
	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaTime);

	virtual void SetupInputComponent() override;

	void ShowReturnToMainMenu();

	UFUNCTION(Client, Reliable)
	void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);
private:
	UPROPERTY()
	class ABlasterHUD* BlasterHUD;
	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float LevelStartingTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	bool bInitializeCharacterOverlay = false;
	float HUDHealth;
	bool bInitializeHealth = false;
	float HUDMaxHealth;
	float HUDScore;
	bool bInitializeScore = false;
	int32 HUDDefeats;
	bool bInitializeDefeats = false;
	float HUDCarriedAmmo;
	bool bInitializeCarriedAmmo = false;
	float HUDWeaponAmmo;
	bool bInitializeWeaponAmmo = false;

	float HighPingRunningTime = 0.f;
	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;
	float PingAnimationRunningTime = 0.f;
	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 20.f;
	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool bHighPing);
	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 50.f;

	UPROPERTY(EditAnywhere, Category = HUD)
	TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;

	UPROPERTY()
	class UReturnToMainMenu* ReturnToMainMenu;

	bool bReturnToMainMenuOpen = false;

};
