#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimation.generated.h"

/**
 * 
 */
UCLASS()
class MI498_UEPROJECT_API UEnemyAnimation : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	float Speed;
};
