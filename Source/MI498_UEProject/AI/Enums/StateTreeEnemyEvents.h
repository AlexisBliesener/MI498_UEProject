#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType, meta = (DisplayName = "State Tree Events"))
enum class StateTreeEnemyEvents : uint8
{
    Unknown    UMETA(DisplayName = "Unknown"),
    Idle       UMETA(DisplayName = "Idle"),
    Patrol     UMETA(DisplayName = "Patrol"),
    Attack     UMETA(DisplayName = "Attack")
};