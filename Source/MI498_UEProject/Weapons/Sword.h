#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "GameFramework/Actor.h"
#include "Sword.generated.h"

UCLASS(Blueprintable)
class MI498_UEPROJECT_API ASword : public AWeaponBase
{

public:
	/// Override the PrimaryAttack function to implement Sword specific attacking behavior
	/// @param PlayerController - Provides context about who is attaclomg 
	virtual void PrimaryAttack(APlayerController* PlayerController) override;

protected:

private:
	GENERATED_BODY()
};
