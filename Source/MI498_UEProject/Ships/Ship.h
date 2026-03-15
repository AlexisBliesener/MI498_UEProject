#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ship.generated.h"

/// Delegate that calls when the ship begins to falls
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnShipFall,
	float,
	speed
);

/// falls downward when triggered.
UCLASS()
class MI498_UEPROJECT_API AShip : public AActor
{
	GENERATED_BODY()

public:
	AShip();

	/// Speed at which the ship falls (units per second).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int FallSpeed = 100;
	
	/// Event broadcast when the ship begins to fall
	FOnShipFall OnShipFall;

	/// Starts the falling behavior.
	void StartFalling() { bFalling = true; }

protected:
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;
	/// Applies downward movement each frame.
	void Fall(float DeltaTime);
	/**
	 * it makes a copy ship far under the map and 
	 * moves the nav mesh on the ship to the copy one so the ai can use it 
	 */
	void DuplicateShipForNavigation();

private:
	/// Tracks whether the ship is currently falling.
	bool bFalling = false;
};