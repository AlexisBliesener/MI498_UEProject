#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OutsideVaultDoor.generated.h"

UCLASS()
class MI498_UEPROJECT_API AOutsideVaultDoor : public AActor
{
	GENERATED_BODY()

public:
	/// Blueprint event that calls to lock the door
	UFUNCTION(BlueprintImplementableEvent)
	void LockDoor();
	
	/// Blueprint event that calls to unlock the door
	UFUNCTION(BlueprintImplementableEvent)
	void UnlockDoor();
	
};
