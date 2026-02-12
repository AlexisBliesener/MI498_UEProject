#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VaultRoom.generated.h"

class UBoxComponent;

/// Dynamic multicast delegate fired when the player enters or exits the vault room
/// Parameter indicates whether the player is currently inside the vault
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnInVaultStatusChange,
	bool,
	bIsInVault
);

/// Actor representing a vault room trigger zone
/// Detects player overlap and broadcasts vault-entry status changes
UCLASS()
class MI498_UEPROJECT_API AVaultRoom : public AActor
{
public:
	AVaultRoom();
	
	/// Event broadcast whenever the player enters or leaves the vault room
	/// true = player inside, false = player outside
	FOnInVaultStatusChange OnVaultDoorInteract;
	
	/// Updates and broadcasts the player vault status
	void InVaultStatusChange(bool Status) const;

protected:
	virtual void BeginPlay() override;

private:
	/// Called when another actor begins overlapping the trigger volume
	UFUNCTION()
	void OnRoomBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	/// Called when another actor stops overlapping the trigger volume
	UFUNCTION()
	void OnRoomEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
	
	/// Box collision component used as the vault room trigger volume
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> RoomTrigger;

	GENERATED_BODY()
};
