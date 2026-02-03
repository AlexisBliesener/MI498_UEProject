#pragma once

#include "CoreMinimal.h"
#include "../WeaponBase.h"
#include "GameFramework/Actor.h"
#include "Sword.generated.h"

/// Concrete weapon class representing a Sword
/// /// Inherits from WeaponBase
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
	
	/// How much force is applied to the player when dashing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int DashForce = 700;
	
	/// Cooldown duration (in seconds) between secondary attacks
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SecondaryCooldown = 1;

protected:

private:
	/// Performs the actual sword swing hit detection
	/// Uses a forward sweep to detect and damage actors in range
	/// @param Controller - Controller performing the swing
	/// @param Target - Optional target actor reference
	void SwingSword(AController* Controller, AActor* Target = nullptr);
	
	/// Timestamp of the last time the secondary attack was used
	/// Used to enforce cooldown timing
	float SecondaryCooldownTimer = 0;
	
	GENERATED_BODY()
};
