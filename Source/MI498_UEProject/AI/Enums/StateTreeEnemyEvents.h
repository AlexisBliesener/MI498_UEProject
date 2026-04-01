#pragma once

#include "CoreMinimal.h"

/**
 * enum for state transitions within the State Tree system.
 */
UENUM(BlueprintType, meta = (DisplayName = "State Tree Events"))
enum class StateTreeEnemyEvents : uint8
{
    Unknown    UMETA(DisplayName = "Unknown"),
    Idle       UMETA(DisplayName = "Idle"),
    Patrol     UMETA(DisplayName = "Patrol"),
    Attack     UMETA(DisplayName = "Attack"),
    Search     UMETA(DisplayName = "Search"),
    Stunned    UMETA(DisplayName = "Stunned"),
    StunEnd    UMETA(DisplayName = "StunEnd")
};