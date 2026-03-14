#pragma once

#include "CoreMinimal.h"
#include "../WeaponBase.h"
#include "GameFramework/Actor.h"
#include "Sword.generated.h"

/// Concrete weapon class representing a Sword
/// Inherits from WeaponBase
UCLASS()
class MI498_UEPROJECT_API ASword : public AWeaponBase
{

public:
	ASword();
	
	/// Override the PrimaryAttack function to implement Sword specific attacking behavior
	/// @param Controller - Provides context about who is attacking 
	virtual void PrimaryAttack(AController* Controller, AActor* Target = nullptr) override;
	
	/// Executes repeated or held primary attack behavior
	/// Used when the input button is held down
	/// @param Controller - Controller responsible for the attack
	/// @param Target - Optional intended target actor
	virtual void PrimaryAttackHold(AController* Controller, AActor* Target = nullptr) override;
	
	/// Override the Secondary Attack function to implement Sword specific attacking behavior
	/// @param Controller - Provides context about who is attacking 
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void SecondaryAttack(AController* Controller, AActor* Target = nullptr) override;
	
	/// How much force is applied to the player when dashing
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int DashForce = 700;
	
	/// Reload time for sword dash charges
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SecondaryReloadTime = 0.2;
	
	/// Cooldown time inbetween using sword dashses
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SecondaryCooldownTime = 0.2;
	
	/// The max amount of dashes to be had at any time
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int DashCharges = 3;
	
	/// How many seconds of invincibility the player will get after using the dash
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DashInvincibilitySeconds = 0.5f;
	
	/// Amount of damage applied to actors hit during the dash
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int DashDamage = 6;
	
	/// Strength of knockback in forward and upward direction added to an enemy when hit
	FVector2D EnemyKnockbackForce = FVector2D(800, 500);

protected:
	virtual void Tick(float DeltaSeconds) override;

	/// Current remaining dash charges.
	UPROPERTY(BlueprintReadOnly)
	int CurrentDashCharges = DashCharges;
	
	/// Tracks whether the next attack is the first in a combo sequence.
	UPROPERTY(BlueprintReadOnly)
	bool bFirstAttackInSequence = true;
	
private:
	/// Performs the actual sword swing hit detection
	/// Uses a forward sweep to detect and damage actors in range
	/// @param Controller - Controller performing the swing
	/// @param Target - Optional target actor reference
	void SwingSword(AController* Controller, AActor* Target = nullptr);
	
	/// Refills dash charges back to maximum and re-enables dash use.
	UFUNCTION()
	void ReloadDashes();
	
	/// Sets whether the secondary ability can be used.
	UFUNCTION()
	void SetCanUseSecondary(const bool Val) {bCanUseSecondary = Val;}
	
	/// Activate the sword dash hitbox
	void DashHitbox();
	
	/// Timer handle used to reset the combo sequence
	FTimerHandle ComboResetTimer;
	
	/// Timer handle for dash cooldown between uses.
	FTimerHandle SecondaryCooldownTimerHandle;
	
	/// Timer handle for dash charge reload delay.
	FTimerHandle SecondaryReloadTimerHandle;
	
	/// Whether the secondary ability is currently allowed.
	bool bCanUseSecondary = true;
	
	/// Time window (in seconds) before combo resets
	float ComboResetTime = 0.6f;
	
	/// Whether a dash reload timer is currently running.
	bool bReloadingSecondary = false;
	
	/// Resets the combo sequence back to the first attack.
	void ResetCombo();
	
	/// Whether the sword dash hitbox is currently active
	bool bSwordDashHitboxActive = false;

	/// Duration the dash hitbox remains active
	float SwordDashHitboxDuration = 0.25f;

	/// Time when the dash hitbox was activated
	float SwordDashHitboxStartTime = 0.f;

	/// Tracks actors already hit during the current dash
	/// Prevents hitting the same actor multiple times
	UPROPERTY()
	TSet<AActor*> DashHitActors;
	
	GENERATED_BODY()
};
