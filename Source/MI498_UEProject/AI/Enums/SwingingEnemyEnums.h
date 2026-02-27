#pragma once

#include "CoreMinimal.h"

/**
 * enum for state transitions within the State Tree system.
 */
UENUM(BlueprintType, meta = (DisplayName = "State Tree Events"))
enum class SwingingEnemyEnums : uint8
{
    Idle    UMETA(DisplayName = "Idle State"),
    Swing    UMETA(DisplayName = "Swing State"),
    ThrowABomb    UMETA(DisplayName = "Jump and Throw a bomb State"),
    Chase       UMETA(DisplayName = "Chase The Player"),
    Melee     UMETA(DisplayName = "Melee Attack")
};