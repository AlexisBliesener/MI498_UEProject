#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponManager.generated.h"

class UInputMappingContext;
class UInputAction;
class APlayerCharacter;
struct FInputActionValue;
class IWeaponInterface;

/// Declare a logging category specifically for the weapon manager
DECLARE_LOG_CATEGORY_EXTERN(WeaponLog, Log, All);

/// Component responsible for managing weapons for a player character
/// Handles spawning, switching, and input bindings for weapons
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MI498_UEPROJECT_API UWeaponManager : public UActorComponent
{

public:	
	/// Input actions for selecting individual weapons
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputAction> ActionWeaponOne = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputAction> ActionWeaponTwo = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputAction> ActionWeaponThree = nullptr;
	
	/// Input actions for cycling through weapons
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputAction> ActionWeaponNext = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputAction> ActionWeaponPrev = nullptr;
	
	/// Input actions for attacking with the current weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputAction> ActionPrimaryAttack = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputAction> ActionSecondaryAttack = nullptr;
	
	/// Input mapping context applied when this controller possesses a pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;
	
	/// Array of weapon blueprints that will be spawned for the player
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TArray<TSubclassOf<AActor>> WeaponBlueprints;

protected:
	/// Called when the component is initialized at game start
	virtual void BeginPlay() override;
	
	/// Handlers for weapon selection actions
	void HandleSelectWeaponOne();
	void HandleSelectWeaponTwo();
	void HandleSelectWeaponThree();
	void HandleSelectWeaponPrev();
	void HandleSelectWeaponNext();
	
	/// Handlers for primary and secondary attack actions
	void HandlePrimaryAttack();
	void HandleSecondaryAttack();

private:
	/// Enhanced input component used for binding input actions
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = nullptr;
	
	/// Reference to the possessed PlayerCharacter
	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter = nullptr;
	
	/// Array of spawned weapon instances as interfaces for generic access
	UPROPERTY()
	TArray<TScriptInterface<IWeaponInterface>> WeaponOptions;
	
	/// The currently selected weapon
	UPROPERTY(VisibleAnywhere)
	TScriptInterface<IWeaponInterface> currentWeapon;

	GENERATED_BODY()
};
