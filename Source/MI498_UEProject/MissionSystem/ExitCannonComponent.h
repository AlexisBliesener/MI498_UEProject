#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExitCannonComponent.generated.h"

class UBoxComponent;

/// Fired when the player enters the exit cannon trigger volume
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNearExitCannon);

/// Fired when the player is launched from the cannon
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShotFromCannon);

/// Exit Cannon Component
/// Handles trigger detection and event broadcasting for the exit cannon
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MI498_UEPROJECT_API UExitCannonComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/// Event broadcast when the player is near the exit cannon
	UPROPERTY()
	FOnNearExitCannon OnNearExitCannon;
	
	/// Multicast event broadcast when the player is shot from the cannon
	UPROPERTY()
	FOnShotFromCannon OnShotFromCannon;
	
	/// Called when the player is near the exit cannon
	/// Broadcasts the enter event
	void NearExitCannon() const;
	
	/// Triggers the shot event
	UFUNCTION(BlueprintCallable)
	void ShotFromCannon() const;

protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	/// Handler for trigger begin-overlap events
	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
