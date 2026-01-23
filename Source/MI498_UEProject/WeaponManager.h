#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponManager.generated.h"

class UInputMappingContext;
class UInputAction;
class APlayerCharacter;
struct FInputActionValue;
class IWeaponInterface;

DECLARE_LOG_CATEGORY_EXTERN(WeaponLog, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MI498_UEPROJECT_API UWeaponManager : public UActorComponent
{

public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputAction> ActionWeaponOne = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputAction> ActionWeaponTwo = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputAction> ActionWeaponThree = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputAction> ActionWeaponNext = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputAction> ActionWeaponPrev = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputAction> ActionPrimaryAttack = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputAction> ActionSecondaryAttack = nullptr;
	
	/// Input mapping context applied when this controller possesses a pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Selection")
	TArray<TSubclassOf<AActor>> WeaponBlueprints;

protected:
	
	virtual void BeginPlay() override;
	
	void HandleSelectWeaponOne();
	void HandleSelectWeaponTwo();
	void HandleSelectWeaponThree();
	void HandleSelectWeaponPrev();
	void HandleSelectWeaponNext();
	void HandlePrimaryAttack();
	void HandleSecondaryAttack();

private:
	/// Enhanced input component used for binding input actions
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = nullptr;
	
	/// Reference to the possessed PlayerCharacter
	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter = nullptr;
	
	UPROPERTY()
	TArray<TScriptInterface<IWeaponInterface>> WeaponOptions;
	
	UPROPERTY(VisibleAnywhere)
	TScriptInterface<IWeaponInterface> currentWeapon;

	GENERATED_BODY()
};
