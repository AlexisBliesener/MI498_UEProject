#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "../WeaponBase.h"
#include "GameFramework/Actor.h"
#include "Blunderbuss.generated.h"

class APlayerController;

/// A concrete weapon class representing a Blunderbuss 
/// Inherits from WeaponBase
UCLASS()
class MI498_UEPROJECT_API ABlunderbuss : public AWeaponBase
{
public:	
	ABlunderbuss();
	
	/// Override the PrimaryAttack function to implement Blunderbuss-specific firing behavior
	/// @param Controller - Provides context about who is firing 
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void PrimaryAttack(AController* Controller, AActor* Target = nullptr) override;
	
	/// Called when the primary attack input is held
	/// Not currently used for the blunderbuss
	/// @param Controller - Controller responsible for firing
	/// @param Target - Optional target actor reference
	virtual void PrimaryAttackHold(AController* Controller, AActor* Target = nullptr) override;
	
	/// Override the SecondaryAttack function to implement Blunderbuss-specific firing behavior
	/// @param Controller - Provides context about who is firing 
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void SecondaryAttack(AController* Controller, AActor* Target = nullptr) override;
	
	/// How much time it will take to reload the blunderbuss if it is missing two ammo
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int TwoAmmoReloadTime = 1;
	
	/// How much force is applied to the player when firing the weapon in air during the primary attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int PrimaryAttackKnockbackForce = 500;
	
	/// How much force is applied to the player when firing the weapon in air during the secondary attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int SecondaryAttackKnockbackForce = 1000;
	
	/// Damage multiplier applied to the secondary (double) shot
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DoubleShotDamageMultiplier = 2;
	
	/// Float curve that defines how recoil is applied over time for the primary shot
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCurveFloat* PrimaryRecoilCurve;
	
	/// Float curve that defines how recoil is applied over time for the secondary shot.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCurveFloat* SecondaryRecoilCurve;
	
	/// Strength of knockback in forward and upward direction added to an enemy when hit	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D EnemyKnockbackForcePrimary = FVector2D(800, 500);
	
	/// Strength of knockback in forward and upward direction added to an enemy when hit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	FVector2D EnemyKnockbackForceSecondary = FVector2D(1200, 600);
	
	/// The vfx that will be played when the blunderbuss shot hits something
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* HitVFX;
	
	/// The beam vfx that will be used to make the blunderbuss tracer lines
	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* TracerVFX;
	
	/// An event that will call when the blunderbuss is fired with not enough ammo
	UFUNCTION(BlueprintImplementableEvent)
	void OnFiredWithNotEnoughAmmo();

protected:
	virtual void BeginPlay() override;

private:
	/// Applies physical knockback and camera recoil to the firing player
	/// Launches the character opposite their view direction if airborne
	/// @param PlayerController - Controller of the firing player
	/// @param KnockbackForce - Strength of backward launch
	void PlayerKnockback(APlayerController* PlayerController, int KnockbackForce) const;
	
	/// Applies camera recoil over time using a curve and timer.
	/// Handles both primary and secondary recoil behavior.
	/// @param PlayerController - Controller of the firing player
	/// @param Primary - True if using primary recoil curve, false for secondary
	void ApplyCameraRecoil(APlayerController* PlayerController, bool Primary);
	
	/// Performs the actual hitscan/sweep fire logic
	/// Calculates trace, detects hit, applies falloff damage
	/// @param Controller - Controller that initiated the shot
	/// @param Target - Target reference
	/// @param Damage - Base damage before falloff calculation
	/// @param EnemyKnockbackForce - The amount of force that will push the enemy away and up
	void Fire(AController* Controller, AActor* Target, int Damage, FVector2D EnemyKnockbackForce);
	
	/// Timer handle used to repeatedly apply recoil over time.
	FTimerHandle RecoilTimerHandle;
	
	/// Current step index while iterating through recoil curve.
	int CurrentRecoilStep = 0;
	
	/// Tracks the current time value being sampled from the recoil curve.
	float CurrentRecoilTime = 0;
	
	/// Indicates whether recoil is in reset phase (returning camera).
	bool bResetRecoil = false;
	
	/// Total number of discrete steps used to evaluate recoil curve.
	int RecoilSteps = 50;
	
	/// Total duration of the recoil curve.
	/// Determined dynamically from the last keyframe.
	float RecoilTime = 0;
	
	/// The time it takes to reload one ammo
	float OneAmmoReloadTime = 0.5f;
	
	/// Caches mesh of the player
	UPROPERTY()
	USkeletalMeshComponent* PlayerMesh;
	
	GENERATED_BODY()
};
