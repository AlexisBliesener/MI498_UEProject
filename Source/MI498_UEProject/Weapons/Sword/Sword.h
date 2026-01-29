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
	
	/// Override the Secondary Attack function to implement Sword specific attacking behavior
	/// @param Controller - Provides context about who is attacking 
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void SecondaryAttack(AController* Controller, AActor* Target = nullptr) override;
	
	/// How much force is applied to the player when dashing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int DashForce = 700;

protected:

private:
	GENERATED_BODY()
};
