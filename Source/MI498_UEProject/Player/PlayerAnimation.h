#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimation.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Blunderbuss,
	Sword,
	HarpoonGun
};


/// Animation Instance class for the player character.
/// Responsible for updating animation-related variables
/// that are exposed to the Animation Blueprint.
UCLASS()
class MI498_UEPROJECT_API UPlayerAnimation : public UAnimInstance
{
public:
	/// The speed of the player
	UPROPERTY(BlueprintReadOnly)
	float Speed;
	
	UPROPERTY(BlueprintReadOnly)
	FVector BlunderbussBoneScale = FVector(1.0f);
	
	UPROPERTY(BlueprintReadOnly)
	FVector SwordBoneScale = FVector(0);
	
	UPROPERTY(BlueprintReadOnly)
	FVector HarpoonGunBoneScale = FVector(0);
	
	void SetCurrentWeapon(EWeaponType Weapon);
	
	void SetInAir(const bool Val) {bInAir = Val;}
	void SetJumped(const bool Val) {bJumped = Val;}
	
protected:
	
	UPROPERTY(BlueprintReadOnly)
	bool bJumped = false;
	UPROPERTY(BlueprintReadOnly)
	bool bInAir = false;

private:
	
	
	GENERATED_BODY()
};
