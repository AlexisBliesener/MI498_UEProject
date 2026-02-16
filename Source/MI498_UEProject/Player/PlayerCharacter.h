#pragma once

#include "CoreMinimal.h"
#include "MI498_UEProject/Characters/CharacterBase.h"
#include "PlayerCharacter.generated.h"


class UWeaponManager;
class UCameraComponent;
class UScoringManager;

/// Character class representing the player-controlled pawn.
///
/// Handles player-specific movement behavior such as walking and sprinting.
UCLASS()
class MI498_UEPROJECT_API APlayerCharacter : public ACharacterBase
{
public:
	
	APlayerCharacter();
	
	// Called when ApplyDamage is used
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,AActor* DamageCauser) override;

	/// First person camera
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
	
	/// A Blueprintable function that will be called when the player lands on the ground
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerLanded();
	
	/// A Blueprintable function that will be called when the player starts sprinting
	UFUNCTION(BlueprintImplementableEvent)
	void OnSprint();
	
	/// A Blueprintable function that will be called when the player's health hits 0
	UFUNCTION(BlueprintImplementableEvent, Category="Player")
	void OnPlayerDied();
	
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
	
protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
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
	float MaxDistanceFromLedge = 50;
	
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
	float PullUpToLedgeForce = 700;
	
	/// Forward launch strength used to move player onto ledge
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StepForwardToLedgeForce = 400;
	
	/// Component responsible for managing the player's weapons
	/// Handles spawning, switching, and firing weapons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UWeaponManager> WeaponManager = nullptr;

private:
	/// Called automatically by the engine when the character lands on the ground
	/// Used to trigger Blueprint landing events
	virtual void Landed(const FHitResult& Hit) override;
	
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
	
	/// Whether the player is currently invincible
	bool bIsInvincible = false;
	
	/// World time when invincibility expires
	float InvincibilityTimer = 0.0f;
	
	/// If the camera FOV is being overriden (ADS)
	bool bOverrideCameraFOV = false;
	
	/// What value the camera FOV is being overriden to if enabled
	int OverrideCameraFOV = 90;
	
	GENERATED_BODY()
};
