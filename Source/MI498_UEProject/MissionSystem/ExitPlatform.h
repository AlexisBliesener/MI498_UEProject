#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExitPlatform.generated.h"

class UBoxComponent;

/// Dynamic multicast delegate fired when the player enters the exit platform trigger
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterExitPlatform);

UCLASS()
class MI498_UEPROJECT_API AExitPlatform : public AActor
{

public:
	AExitPlatform();
	
	/// Event broadcast when the exit platform is entered
	FOnEnterExitPlatform OnEnterExitPlatform;
	
	/// Called when the exit platform is entered
	/// Broadcasts the enter event
	void EnterExitPlatform() const;

protected:
	virtual void BeginPlay() override;

private:
	/// Box collision component used as the platform trigger volume
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> RoomTrigger;

	/// Handler for trigger begin-overlap events
	UFUNCTION()
	void OnRoomBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	GENERATED_BODY()
};
