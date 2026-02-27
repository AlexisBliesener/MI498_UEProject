#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.generated.h"

/// Enum representing all weapon categories in the game.
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Other        UMETA(DisplayName = "Other"),
	Blunderbuss  UMETA(DisplayName = "Blunderbuss"),
	Sword        UMETA(DisplayName = "Sword"),
	HarpoonGun   UMETA(DisplayName = "Harpoon Gun")
};