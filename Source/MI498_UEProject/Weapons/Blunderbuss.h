#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "GameFramework/Actor.h"
#include "Blunderbuss.generated.h"

class APlayerController;

/// A concrete weapon class representing a Blunderbuss 
/// Inherits from WeaponBase and implements PrimaryAttack
UCLASS(Blueprintable)
class MI498_UEPROJECT_API ABlunderbuss : public AWeaponBase
{
public:	
	/// Override the PrimaryAttack function to implement Blunderbuss-specific firing behavior
	/// @param PlayerController - Provides context about who is firing 
	virtual void PrimaryAttack(APlayerController* PlayerController) override;
	
	/// How much force is applied to the player when firing the weapon in air
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int KnockbackForce = 500;
	
	/// Vertical camera recoil applied to the player when firing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int CameraRecoil = -5;

protected:

private:
	GENERATED_BODY()
};
