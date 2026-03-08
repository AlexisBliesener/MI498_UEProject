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
	AHarpoonGun();
	
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
	
	/// Player jumped while holding the harpoon gun
	virtual void JumpAction() override;
	
	/// Returns whether the harpoon gun is currently in swing mode.
	/// When true, the player will swing from the harpoon instead of zipping directly to it.
	/// @return True if swing mode is active, false if zip mode is active
	bool IsSwingMode() const {return bSwingMode;}
	
	/// Destroys the currently active harpoon if one exists
	void DestroyCurrentHarpoon();
	
	/// Override for the reload function from WeaponBase, pulls back the harpoon
	virtual void Reload() override;
	
	/// Blueprint class used to spawn the harpoon projectile
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHarpoon> HarpoonBlueprint;
	
	/// FOV will change to this value when ADS
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int ADSFOV = 40;
	
	/// The percent movement and look will slow by when using ADX
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float ADSSlowMovementPercentage = 0.4;
	
	/// The force added to the player when they release the harpoon with a jump
	UPROPERTY(EditDefaultsOnly)
	int HarpoonReleaseJumpForce = 600;

protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

private:
	/// Called when the player switches to or from this weapon.
	UFUNCTION()
	void OnWeaponSwitched();
	
	/// Reference to the currently spawned harpoon instance
	UPROPERTY()
	TObjectPtr<AHarpoon> CurrentHarpoon = nullptr;
	
	/// True if player should swing on harpoon, false if player should zip to harpoon
	bool bSwingMode = true;
	
	/// True when the player is currently aiming down sights.
	bool bUsingADS = false;
	
	/// Cached reference to the owning player character.
	UPROPERTY()
	APlayerCharacter* CharacterOwner = nullptr;
	
	float HoldTime = 0.1;
	float HeldTime = 0;
	bool bHolding = false;
	
	GENERATED_BODY()
};
