#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

/// Declares a new Unreal Engine interface class for weapons
UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/// This is the actual interface that other weapon classes will implement
class MI498_UEPROJECT_API IWeaponInterface
{
public:
	/// Called to use this weapons primary attack
	/// @param Controller - Provides context about who is performing the attack
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void PrimaryAttack(AController* Controller, AActor* Target = nullptr) = 0;
	
	/// Executes the held/continuous version of the primary attack
	/// Called when the primary fire input is held down
	/// @param Controller - Controller responsible for performing the attack
	/// @param Target - Optional intended target actor
	virtual void PrimaryAttackHold(AController* Controller, AActor* Target = nullptr) = 0;
	
	/// Executes when primary attack hold is started
	/// @param Controller - Controller responsible for performing the attack
	/// @param Target - Optional intended target actor
	virtual void PrimaryAttackHoldStart(AController* Controller, AActor* Target = nullptr) = 0;
	
	/// Executes when primary attack hold is ended
	/// @param Controller - Controller responsible for performing the attack
	/// @param Target - Optional intended target actor
	virtual void PrimaryAttackHoldEnd(AController* Controller, AActor* Target = nullptr) = 0;
	
	/// Called to use this weapons secondary attack
	/// @param Controller - Provides context about who is performing the 
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void SecondaryAttack(AController* Controller, AActor* Target = nullptr) = 0;
	
	/// Executes when secondary attack hold is started
	/// @param Controller - Controller responsible for performing the attack
	/// @param Target - Optional intended target actor
	virtual void SecondaryAttackHoldStart(AController* Controller, AActor* Target = nullptr) = 0;
	
	/// Executes when secondary attack hold is ended
	/// @param Controller - Controller responsible for performing the attack
	/// @param Target - Optional intended target actor
	virtual void SecondaryAttackHoldEnd(AController* Controller, AActor* Target = nullptr) = 0;
	
	/// Executes when the jump action is pressed
	virtual void JumpAction() = 0;
	
	/// Trigger the weapon’s reload behavior
	virtual void Reload() = 0;

protected:
	
private:
	GENERATED_BODY()
};
