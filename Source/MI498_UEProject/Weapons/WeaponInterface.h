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
	
	/// Called to use this weapons secondary attack
	/// @param Controller - Provides context about who is performing the 
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void SecondaryAttack(AController* Controller, AActor* Target = nullptr) = 0;

protected:
	
private:
	GENERATED_BODY()
};
