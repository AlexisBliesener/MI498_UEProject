#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "../Animation/PlayerAnimation.h"
#include "MI498_UEProject/Characters/CharacterBase.h"
#include "Engine/DamageEvents.h"
#include "Perception/AISightTargetInterface.h"
#include "PlayerCharacter.generated.h"


class UWeaponManager;
class UCameraComponent;
class UScoringManager;

/// Character class representing the player-controlled pawn.
///
/// Handles player-specific movement behavior such as walking and sprinting.
UCLASS()
class MI498_UEPROJECT_API APlayerCharacter : public ACharacterBase, public IGenericTeamAgentInterface, public IAISightTargetInterface 
{
public:
	
	APlayerCharacter();
	
	// Called when ApplyDamage is used
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,AActor* DamageCauser) override;

	/// First person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* Camera;
	
	/// Returns the walk speed of the player
	int GetMaxWalkSpeed() const {return MaxWalkSpeed;}
	
	/// Event that will turn on low health effect in blueprint
	UFUNCTION(BlueprintImplementableEvent)
	void TurnOnLowHealthEffect();
	
	/// Event that will turn off low health effect in blueprint
	UFUNCTION(BlueprintImplementableEvent)
	void TurnOffLowHealthEffect();
	
	/// A Blueprintable function that will be called when the player lands on the ground
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerLanded();
	
	/// A Blueprintable function that will be called when the player starts sprinting
	UFUNCTION(BlueprintImplementableEvent)
	void OnSprint();
	
	/// A Blueprintable function that will be called when the player's health hits 0
	UFUNCTION(BlueprintImplementableEvent, Category="Player")
	void OnPlayerDied();

	// A blueprint callable function that will revive the player for respawn
	UFUNCTION(BlueprintCallable, Category = "Player")
	void RevivePlayer(float ReviveHealth);

	// A blueprint function that is called when the player is revived.
	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
	void OnPlayerRevived();
	
	/// A Blueprintable function that will be called when the player takes damage
	UFUNCTION(BlueprintImplementableEvent, Category="Player")
	void OnPlayerTakeDamage();

	// A Blueprintable function that will be called for damage indicator sake
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnDamageIndicator(AActor* DamageSource);
	
	/// A Blueprintable function that will be called when the player ledge grabs
	UFUNCTION(BlueprintImplementableEvent, Category="Player")
	void OnPlayerGrabLedge();
	
	/// A Blueprintable function that will be called when the player pulls up from the ledge grab
	UFUNCTION(BlueprintImplementableEvent, Category="Player")
	void OnPullUp();
	
	/// Grants temporary invincibility for the specified duration
	/// @param Seconds - How long invincibility should last
	void AddInvincibility(float Seconds);
	
	/// Enables or disables overriding the camera’s field of view (FOV).
	/// @param bOverride - Whether the override FOV should be active
	/// @param OverrideValue - Target FOV to use when override is enabled (ignored if bOverride is false)
	void SetOverrideCameraFOV(const bool bOverride, const int OverrideValue = 0)
	{
		bOverrideCameraFOV = bOverride;
		OverrideCameraFOV = OverrideValue;
	}
	
	/// Get if the player has died
	bool GetDead() {return bDied;}
	
	virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(const FCanBeSeenFromContext& Context,FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested,float& OutSightStrength, int32* UserData = nullptr,const FOnPendingVisibilityQueryProcessedDelegate* Delegate = nullptr) override;
	/// Returns the weapons manager
	UWeaponManager* GetWeaponManager() {return WeaponManager;}
	
	/// Override of the ACharacter Jump functions
	virtual void Jump() override;
	// Override of the team id and set it to 0 for the player 
	virtual FGenericTeamId GetGenericTeamId() const override;
protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void HealCharacter(float HealAmount) override;
	
	/// Scene component used as the start point for the upper ledge detection trace
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* GrabRaycastOrigin;
	
	/// Scene component used as the start point for the body collision trace
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* BodyRaycastOrigin;
	
	/// Forward trace distance used to check if there is empty space above a ledge
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinLedgeSize = 200;
	
	/// Forward trace distance used to detect a wall at body level
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistanceFromLedge = 100;
	
	/// Min field of view
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinFOV = 90;
	
	/// Max field of view
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxFOV = 180;
	
	/// Camera lerp to new FOV speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.01", ClampMax="0.99", UIMin="0.01", UIMax="0.99"))
	float LerpToNewFOVSpeed = 0.2;
	
	/// How to interpolate over FOV
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* FOVCurve;
	
	/// The max velocity that the player will be allowed to travel at
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxVelocity = 10000;
	
	/// Maximum walking speed when the player is not sprinting
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	int MaxWalkSpeed = 400;
	
	/// Upward launch strength used during ledge pull-up step
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PullUpToLedgeForce = 730;
	
	/// Forward launch strength used to move player onto ledge
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StepForwardToLedgeForce = 400;
	
	/// Component responsible for managing the player's weapons
	/// Handles spawning, switching, and firing weapons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UWeaponManager> WeaponManager = nullptr;
	
	/// Returns the players animation script
	UFUNCTION(BlueprintCallable)
	UPlayerAnimation* GetPlayerAnimation() {return PlayerAnimation;}

	/// Default Sight Collision Channel this will be getting from the project settings
	TEnumAsByte<ECollisionChannel> DefaultSightCollisionChannel;
private:
	/// Called automatically by the engine when the character lands on the ground
	/// Used to trigger Blueprint landing events
	virtual void Landed(const FHitResult& Hit) override;
	
	/// Updates the camera position so the camera is staying near the neck while it rotates
	void UpdateCameraOffset() const;

	/// Starts ledge grab sequence
	/// Locks movement and schedules pull-up timer
	void GrabLedge(const FVector& TowardsLedge);
	
	/// Launches player upward
	/// Schedules StepForward
	UFUNCTION()
	void PullUp(const FVector& TowardsLedge);
	
	/// Launches player forward onto ledge
	/// Schedules ReenableMovement
	UFUNCTION()
	void StepForward(const FVector& TowardsLedge);
	
	/// Restores walking movement + player input after climb
	void ReenableMovement();
	
	/// Scoring system instance
	UPROPERTY()
	UScoringManager * ScoringManager = nullptr;
	
	/// True while player is in ledge grab state
	bool bIsGrabbing = false;
	
	/// Timer handle used for ledge grab sequence steps
	FTimerHandle TimerHandle;
	
	/// World time when invincibility expires
	float InvincibilityTimer = 0.0f;
	
	/// If the camera FOV is being overriden (ADS)
	bool bOverrideCameraFOV = false;
	
	/// What value the camera FOV is being overriden to if enabled
	int OverrideCameraFOV = 90;
	
	/// Reference to the players animation controller
	UPROPERTY()
	UPlayerAnimation* PlayerAnimation;
	
	/// If low health was hit
	bool bLowHealthHit;
	
	/// How low health must be to trigger low health effect in percentage of health
	float LowHealthPercentage = 0.1f;
	
	/// Timer that controls when low health effect will turn off
	FTimerHandle LowHealthTimer;
	
	/// True if the player has died
	bool bDied = false;
	
	/// True if the can fall off ledge check should be running
	bool bCanFallOffLedge = true;
	
	/// The players gravity scale when the game starts
	float StartingGravityScale = 1;
	
	GENERATED_BODY()
};
