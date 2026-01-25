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
	/// /// @param PlayerController - Provides context about who is performing the attack
	virtual void PrimaryAttack(APlayerController* PlayerController) = 0;
	
	/// Called to use this weapons secondary attack
	/// /// @param PlayerController - Provides context about who is performing the attack
	virtual void SecondaryAttack(APlayerController* PlayerController) = 0;

protected:
	
private:
	GENERATED_BODY()
};
