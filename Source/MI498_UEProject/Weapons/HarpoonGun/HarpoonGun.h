#pragma once

#include "CoreMinimal.h"
#include "Harpoon.h"
#include "../WeaponBase.h"
#include "HarpoonGun.generated.h"

/// Weapon that fires and manages a single harpoon projectile
UCLASS()
class MI498_UEPROJECT_API AHarpoonGun : public AWeaponBase
{

public:
	/// Override the PrimaryAttack function to implement HarpoonGun-specific firing behavior
	/// @param Controller - Provides context about who is firing 
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void PrimaryAttack(AController* Controller, AActor* Target = nullptr) override;
	
	/// Called when the primary attack input is held
	/// Not currently used for the harpoon gun
	/// @param Controller - Controller responsible for firing
	/// @param Target - Optional target actor reference
	virtual void PrimaryAttackHold(AController* Controller, AActor* Target = nullptr) override;
	
	/// Called when the primary attack hold input is started
	/// @param Controller - Controller responsible for firing
	/// @param Target - Optional target actor reference
	virtual void PrimaryAttackHoldStart(AController* Controller, AActor* Target = nullptr) override;
	
	/// Called when the primary attack hold input is ended
	/// @param Controller - Controller responsible for firing
	/// @param Target - Optional target actor reference
	virtual void PrimaryAttackHoldEnd(AController* Controller, AActor* Target = nullptr) override;
	
	/// Override the SecondaryAttack function to implement HarpoonGun-specific firing behavior
	/// @param Controller - Provides context about who is firing 
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void SecondaryAttack(AController* Controller, AActor* Target = nullptr) override;
	
	/// Called when the secondary attack hold input is started
	/// @param Controller - Controller responsible for firing
	/// @param Target - Optional target actor reference
	virtual void SecondaryAttackHoldStart(AController* Controller, AActor* Target = nullptr) override;
	
	/// Called when the secondary attack hold input is ended
	/// @param Controller - Controller responsible for firing
	/// @param Target - Optional target actor reference
	virtual void SecondaryAttackHoldEnd(AController* Controller, AActor* Target = nullptr) override;
	
	/// Returns whether the harpoon gun is currently in swing mode.
	/// When true, the player will swing from the harpoon instead of zipping directly to it.
	/// @return True if swing mode is active, false if zip mode is active
	bool IsSwingMode() const {return bSwingMode;}
	
	/// Blueprint class used to spawn the harpoon projectile
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHarpoon> HarpoonBlueprint;
	
	/// FOV will change to this value when ADS
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int ADSFOV = 40;
	
	/// Destroys the currently active harpoon if one exists
	void DestroyCurrentHarpoon();

protected:

private:
	
	/// Reference to the currently spawned harpoon instance
	UPROPERTY()
	TObjectPtr<AHarpoon> CurrentHarpoon = nullptr;
	
	/// True if player should swing on harpoon, false if player should zip to harpoon
	bool bSwingMode = false;
	
	GENERATED_BODY()
};
