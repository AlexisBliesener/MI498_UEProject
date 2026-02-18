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
	AWeaponBase();
	
	/// The effective range of the weapon in Unreal units
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	int Range = 1000;

	/// The Damage amount of the weapon 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	float Damage = 30.0f;
	
	/// Maximum ammo capacity per mag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MaxAmmo = 2;
	
	/// The amount of ammo that a primary attack takes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int PrimaryAttackNeededAmmo = 1;
	
	/// The amount of ammo that a secondary attack takes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int SecondaryAttackNeededAmmo = 2;
	
	/// Time in seconds required to complete a reload
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ReloadTime = 0.5;
	
	/// Getter for Current Ammo for the HUD Blueprint
	UFUNCTION(BlueprintPure, Category="Weapon")
	int GetCurrentAmmo() const { return CurrentAmmo; }

	/// Getter for Max Ammo for the HUD Blueprint
	UFUNCTION(BlueprintPure, Category="Weapon")
	int GetMaxAmmo() const { return MaxAmmo; }
	
protected:
	/// Implementation of the PrimaryAttack function from IWeaponInterface
	/// @param Controller - The controller performing the attack
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	/// This function will define the weapon's primary firing behavior
	virtual void PrimaryAttack(AController* Controller, AActor* Target = nullptr) override;
	
	/// Implementation of the PrimaryAttackHold function from IWeaponInterface
	/// @param Controller - The controller performing the attack
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	virtual void PrimaryAttackHold(AController* Controller, AActor* Target = nullptr) override;
	
	/// Implementation of the PrimaryAttackHoldStart function from IWeapon Interface
	/// @param Controller - Controller responsible for performing the attack
	/// @param Target - Optional intended target actor
	virtual void PrimaryAttackHoldStart(AController* Controller, AActor* Target = nullptr) override;
	
	/// Implementation of the PrimaryAttackHoldEnd function from IWeapon Interface
	/// @param Controller - Controller responsible for performing the attack
	/// @param Target - Optional intended target actor
	virtual void PrimaryAttackHoldEnd(AController* Controller, AActor* Target = nullptr) override;
	
	/// Implementation of the SecondaryAttack function from IWeaponInterface
	/// @param Controller - The controller performing the attack
	/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
	/// This function will define the weapon's secondary firing behavior
	virtual void SecondaryAttack(AController* Controller, AActor* Target = nullptr) override;
	
	/// Implementation of the SecondaryAttackHoldStart function from IWeapon Interface
	/// @param Controller - Controller responsible for performing the attack
	/// @param Target - Optional intended target actor
	virtual void SecondaryAttackHoldStart(AController* Controller, AActor* Target = nullptr) override;
	
	/// Implementation of the SecondaryAttackHoldEnd function from IWeapon Interface
	/// @param Controller - Controller responsible for performing the attack
	/// @param Target - Optional intended target actor
	virtual void SecondaryAttackHoldEnd(AController* Controller, AActor* Target = nullptr) override;

	/// A Blueprintable function that will be called when primary attack is fired
	UFUNCTION(BlueprintImplementableEvent)
	void OnPrimaryAttack();
	
	/// A Blueprintable function that will be called when secondary attack is fired
	UFUNCTION(BlueprintImplementableEvent)
	void OnSecondaryAttack();
	
	/// A Blueprintable function that will be called when reload is called
	UFUNCTION(BlueprintImplementableEvent)
	void OnReload();
	
	/// Starts reload process — sets reload state and timer
	virtual void Reload() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	// Current ammo remaining in the weapon
	/// Initialized to MaxAmmo and refilled on reload
	int CurrentAmmo = MaxAmmo;
	
	/// Whether the weapon is currently reloading
	bool bReloading = false;
	
	/// Timestamp when reload started
	float ReloadTimer = 0;

	/// Update if Ammo Changed for HUD
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAmmoChangedSignature, int, CurrentAmmo, int, MaxAmmo, bool, bIsSecondary);
	UPROPERTY(BlueprintAssignable, Category="Weapon")
	FAmmoChangedSignature OnAmmoChanged;

	
private:	
	GENERATED_BODY()
};
