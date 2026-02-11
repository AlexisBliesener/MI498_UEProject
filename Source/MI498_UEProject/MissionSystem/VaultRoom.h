#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VaultRoom.generated.h"

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnInVaultStatusChange,
	bool,
	bIsInVault
);

UCLASS()
class MI498_UEPROJECT_API AVaultRoom : public AActor
{
public:
	AVaultRoom();
	
	FOnInVaultStatusChange OnVaultDoorInteract;
	
	void InVaultStatusChange(bool Status);
	

protected:
	virtual void BeginPlay() override;


private:
	GENERATED_BODY()
	
	// Trigger volume for the room
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> RoomTrigger;

	UFUNCTION()
	void OnRoomBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnRoomEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

};
