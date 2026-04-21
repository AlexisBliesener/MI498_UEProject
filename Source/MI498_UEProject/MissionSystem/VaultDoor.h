#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VaultDoor.generated.h"

/// Dynamic multicast delegate fired when the vault door is interacted with
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVaultDoorInteract);

/// Actor representing an interactable vault door
UCLASS()
class MI498_UEPROJECT_API AVaultDoor : public AActor
{

public:
	/// Event broadcast when the vault door is interacted with
	FOnVaultDoorInteract OnVaultDoorInteract;
	
	/// Called when the player interacts with the vault door
	/// Broadcasts the interaction event 
	UFUNCTION(BlueprintCallable)
	void VaultDoorInteract();
	
	/// Allows door to be interacted with
	UFUNCTION(BlueprintImplementableEvent)
	void EnableInteract();

	/// Allows to enable or disable vault door state
	UFUNCTION(BlueprintImplementableEvent)
	void SetVaultDoorEnabled (bool Enabled);


protected:


private:
	
	GENERATED_BODY()
};
