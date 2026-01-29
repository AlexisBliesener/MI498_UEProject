#pragma once

#include "CoreMinimal.h"
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
	/// Override the PrimaryAttack function to implement Blunderbuss-specific firing behavior
	/// @param Controller - Provides context about who is firing 
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void PrimaryAttack(AController* Controller, AActor* Target = nullptr) override;
	
	/// Override the SecondaryAttack function to implement Blunderbuss-specific firing behavior
	/// @param Controller - Provides context about who is firing 
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void SecondaryAttack(AController* Controller, AActor* Target = nullptr) override;
	
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
