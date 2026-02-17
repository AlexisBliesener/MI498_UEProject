#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimation.generated.h"

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
	
protected:
	
	
private:
	GENERATED_BODY()
};
