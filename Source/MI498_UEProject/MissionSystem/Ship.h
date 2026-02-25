#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/MovementComponent.h"
#include "Ship.generated.h"

/// falls downward when triggered.
UCLASS()
class MI498_UEPROJECT_API AShip : public AActor
{
	GENERATED_BODY()

public:
	AShip();

	/// Speed at which the ship falls (units per second).
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int FallSpeed = 100;

	/// Starts the falling behavior.
	void StartFalling() { bFalling = true; }

protected:
	virtual void Tick(float DeltaSeconds) override;

	/// Applies downward movement each frame.
	void Fall(float DeltaTime);

private:
	/// Tracks whether the ship is currently falling.
	bool bFalling = false;
};