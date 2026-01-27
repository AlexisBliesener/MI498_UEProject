#pragma once

#include "CoreMinimal.h"
#include "WeaponInterface.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(WeaponLog, Log, All);

/// Base class for all weapons in the game
/// Inherits from AActor so it can exist in the world
/// Implements IWeaponInterface so it can be used generically by WeaponManager
UCLASS()
class MI498_UEPROJECT_API AWeaponBase : public AActor,  public IWeaponInterface
{
public:	
	/// The effective range of the weapon in Unreal units
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	int Range = 1000;

protected:
	/// Implementation of the PrimaryAttack function from IWeaponInterface
	/// @param PlayerController - The player performing the attack
	/// This function will define the weapon's primary firing behavior
	virtual void PrimaryAttack(APlayerController* PlayerController) override;
	
	/// Implementation of the SecondaryAttack function from IWeaponInterface
	/// @param PlayerController - The player performing the attack
	/// This function will define the weapon's secondary firing behavior
	virtual void SecondaryAttack(APlayerController* PlayerController) override;

	/// A Blueprintable function that will be called when primary attack is fired
	UFUNCTION(BlueprintImplementableEvent)
	void OnPrimaryAttack();

private:	
	GENERATED_BODY()

};
