#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MI498_UEProject/Weapons/WeaponTypes.h"
#include "PlayerAnimation.generated.h"

/// Enum representing all weapon transition montages in the game.
UENUM(BlueprintType)
enum class EWeaponTransitionMontageToPlay : uint8
{
	BlunderbussToSword UMETA(DisplayName = "Blunderbuss To Sword"),
	BlunderbussToHarpoonGun UMETA(DisplayName = "Blunderbuss To Harpoon Gun"),

	SwordToBlunderbuss UMETA(DisplayName = "Sword To Blunderbuss"),
	SwordToHarpoonGun UMETA(DisplayName = "Sword To Harpoon Gun"),

	HarpoonGunToBlunderbuss UMETA(DisplayName = "Harpoon Gun To Blunderbuss"),
	HarpoonGunToSword UMETA(DisplayName = "Harpoon Gun To Sword"),

	None UMETA(DisplayName = "None")
};

/// Animation Instance class for the player character.
/// Responsible for updating animation-related variables
/// that are exposed to and read by the Animation Blueprint.
UCLASS()
class MI498_UEPROJECT_API UPlayerAnimation : public UAnimInstance
{
public:
	/// Current movement speed of the character.
	UPROPERTY(BlueprintReadOnly)
	float Speed;

	/// Scale applied to the Blunderbuss weapon bone.
	UPROPERTY(BlueprintReadOnly)
	FVector BlunderbussBoneScale = FVector(1.0f);

	/// Scale applied to the Sword weapon bone.
	UPROPERTY(BlueprintReadOnly)
	FVector SwordBoneScale = FVector(0);

	/// Scale applied to the Harpoon Gun weapon bone.
	UPROPERTY(BlueprintReadOnly)
	FVector HarpoonGunBoneScale = FVector(0);

	/// Updates internal animation state based on the currently equipped weapon.
	void SetCurrentWeapon(EWeaponType Weapon);

	/// Sets whether the character is currently in air.
	void SetInAir(const bool Val) { bInAir = Val; }

	/// Sets whether the character has initiated a jump.
	void SetJumped(const bool Val) { bJumped = Val; }

	/// Sets the look direction of the player for animation movement
	void SetLookRotation(float Val) { LookRotation = Val; }

	/// Returns the correct transition animation for prev and current weapon
	UFUNCTION(BlueprintCallable)
	EWeaponTransitionMontageToPlay GetTransitionMontageToPlay();

protected:
	/// The rotation of the look directions
	UPROPERTY(BlueprintReadOnly)
	float LookRotation = 0;

	/// True when the character has triggered a jump.
	UPROPERTY(BlueprintReadOnly)
	bool bJumped = false;

	/// True while the character is airborne (falling or jumping).
	UPROPERTY(BlueprintReadOnly)
	bool bInAir = false;

private:
	/// The weapon the player held previous to current
	EWeaponType PrevWeapon = EWeaponType::Blunderbuss;
	
	/// The weapon the player is currently holding
	EWeaponType CurrentWeapon = EWeaponType::Blunderbuss;

	GENERATED_BODY()
};
