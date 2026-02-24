#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplodingBarrel.generated.h"


/// Base C++ class for an exploding barrel.
/// The explosion behavior itself is implemented in Blueprint.
UCLASS()
class MI498_UEPROJECT_API AExplodingBarrel : public AActor
{
	GENERATED_BODY()

public:

	/// Blueprint event that is triggered when the barrel explodes.
	UFUNCTION(BlueprintImplementableEvent)
	void OnExplode();

	/// Public function used to trigger the explosion.
	UFUNCTION(BlueprintCallable)
	void Explode() { OnExplode(); }
	
};