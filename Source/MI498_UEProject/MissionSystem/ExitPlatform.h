#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExitPlatform.generated.h"

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterExitPlatform);

UCLASS()
class MI498_UEPROJECT_API AExitPlatform : public AActor
{

public:
	AExitPlatform();
	FOnEnterExitPlatform OnEnterExitPlatform;
	
	
	void EnterExitPlatform();


protected:
	virtual void BeginPlay() override;


private:
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
	
	
	GENERATED_BODY()

};
