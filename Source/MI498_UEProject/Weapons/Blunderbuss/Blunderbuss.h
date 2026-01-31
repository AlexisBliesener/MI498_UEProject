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
	ABlunderbuss();
	
	/// Override the PrimaryAttack function to implement Blunderbuss-specific firing behavior
	/// @param Controller - Provides context about who is firing 
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void PrimaryAttack(AController* Controller, AActor* Target = nullptr) override;
	
	/// Override the SecondaryAttack function to implement Blunderbuss-specific firing behavior
	/// @param Controller - Provides context about who is firing 
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void SecondaryAttack(AController* Controller, AActor* Target = nullptr) override;
	
	virtual void Reload() override;
	
	/// How much force is applied to the player when firing the weapon in air during the primary attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int PrimaryAttackKnockbackForce = 500;
	
	/// How much force is applied to the player when firing the weapon in air during the secondary attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int SecondaryAttackKnockbackForce = 1000;
	
	/// Vertical camera recoil applied to the player when firing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int CameraRecoil = -5;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DoubleShotDamageMultiplier = 2;

protected:
	
	virtual void Tick(float DeltaSeconds) override;

private:
	
	void PlayerKnockback(APlayerController* PlayerController, int KnockbackForce) const;
	void Fire(AController* Controller, AActor* Target, int Damage) const;
	
	GENERATED_BODY()
};
