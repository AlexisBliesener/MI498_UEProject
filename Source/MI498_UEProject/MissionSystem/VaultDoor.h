#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VaultDoor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVaultDoorInteract);

UCLASS()
class MI498_UEPROJECT_API AVaultDoor : public AActor
{

public:
	FOnVaultDoorInteract OnVaultDoorInteract;
	
	UFUNCTION(BlueprintCallable)
	void VaultDoorInteract();


protected:


private:
	
	GENERATED_BODY()

};
