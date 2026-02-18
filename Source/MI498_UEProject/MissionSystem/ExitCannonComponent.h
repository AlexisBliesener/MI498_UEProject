#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExitCannonComponent.generated.h"

class UBoxComponent;

/// Dynamic multicast delegate fired when the player enters the exit platform trigger
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNearExitCannon);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShotFromCannon);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MI498_UEPROJECT_API UExitCannonComponent : public UActorComponent
{

public:
	UExitCannonComponent();
	
	/// Event broadcast when the player is near the exit cannon
	FOnNearExitCannon OnNearExitCannon;
	
	FOnShotFromCannon OnShotFromCannon;
	
	/// Called when the player is near the exit cannon
	/// Broadcasts the enter event
	void NearExitCannon() const;
	
	UFUNCTION(BlueprintCallable)
	void ShotFromCannon() const;

protected:
	virtual void BeginPlay() override;

private:
	/// Box collision component used as the near object trigger volume
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> NearObjectTrigger;

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
	
	GENERATED_BODY()
};
