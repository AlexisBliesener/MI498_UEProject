#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimation.generated.h"

/// Enum representing the currently equipped weapon type.
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Blunderbuss,
	Sword,
	HarpoonGun
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
	
	void SetLookRotation(float Val) { LookRotation = Val; }

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

	GENERATED_BODY()
};