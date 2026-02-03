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
	
	/// Override the SecondaryAttack function to implement HarpoonGun-specific firing behavior
	/// @param Controller - Provides context about who is firing 
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void SecondaryAttack(AController* Controller, AActor* Target = nullptr) override;
	
	/// Blueprint class used to spawn the harpoon projectile
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHarpoon> HarpoonBlueprint;
	
	/// Destroys the currently active harpoon if one exists
	void DestroyCurrentHarpoon();

protected:

private:
	
	/// Reference to the currently spawned harpoon instance
	UPROPERTY()
	TObjectPtr<AHarpoon> CurrentHarpoon = nullptr;
	
	GENERATED_BODY()
};
