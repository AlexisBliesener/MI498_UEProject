#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlantedBomb.generated.h"

UCLASS()
class MI498_UEPROJECT_API APlantedBomb : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void BombAppear();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BombExplode();
	/// Has the bomb cutscene played?  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasCutscenePlayed = false;
};
