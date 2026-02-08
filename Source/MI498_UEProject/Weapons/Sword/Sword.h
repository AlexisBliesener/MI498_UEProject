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
	
	/// Called when the secondary attack input is held
	/// Not currently used for Sword
	/// @param Controller - Controller responsible for firing
	/// @param Target - Optional target actor reference
	virtual void SecondaryAttackHold(AController* Controller, AActor* Target = nullptr) override;
	
	/// How much force is applied to the player when dashing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
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

protected:
	virtual void Tick(float DeltaSeconds) override;

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
	
	/// Timer handle for dash cooldown between uses.
	FTimerHandle SecondaryCooldownTimerHandle;
	
	/// Timer handle for dash charge reload delay.
	FTimerHandle SecondaryReloadTimerHandle;
	
	/// Current remaining dash charges.
	int CurrentDashCharges = DashCharges;
	
	/// Whether the secondary ability is currently allowed.
	bool bCanUseSecondary = true;
	
	/// Whether a dash reload timer is currently running.
	bool bReloadingSecondary = false;
	
	GENERATED_BODY()
};
