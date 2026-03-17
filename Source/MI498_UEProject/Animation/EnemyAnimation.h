#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimation.generated.h"

/// Animation instance used by enemy characters.
UCLASS()
class MI498_UEPROJECT_API UEnemyAnimation : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	/// Current movement speed of the enemy.
	UPROPERTY(BlueprintReadOnly)
	float Speed;
};