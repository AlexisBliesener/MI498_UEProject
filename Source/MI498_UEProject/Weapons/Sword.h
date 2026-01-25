#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "GameFramework/Actor.h"
#include "Sword.generated.h"

/// Concrete weapon class representing a Sword
/// /// Inherits from WeaponBase
UCLASS()
class MI498_UEPROJECT_API ASword : public AWeaponBase
{

public:
	/// Override the PrimaryAttack function to implement Sword specific attacking behavior
	/// @param PlayerController - Provides context about who is attacking 
	virtual void PrimaryAttack(APlayerController* PlayerController) override;
	
	/// Override the Secondary Attack function to implement Sword specific attacking behavior
	/// @param PlayerController - Provides context about who is attacking 
	virtual void SecondaryAttack(APlayerController* PlayerController) override;
	
	/// How much force is applied to the player when dashing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int DashForce = 700;

protected:

private:
	GENERATED_BODY()
};
