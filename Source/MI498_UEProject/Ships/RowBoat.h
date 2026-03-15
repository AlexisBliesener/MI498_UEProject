#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "RowBoat.generated.h"


class AShip;

UCLASS()
class MI498_UEPROJECT_API ARowBoat : public AActor
{
	GENERATED_BODY()

public:
	ARowBoat();

	/// Speed at which the boat moves along the spline path
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 300.f;

	/// Reference to the ship that is attached to this boat, this rowboat will fall with this ship
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AShip* ConnectedShip = nullptr;

protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	/// Handles falling movement when triggered
	void Fall(float DeltaTime);

private:
	/// Reference to the spline component that defines the boat's movement path
	UPROPERTY()
	USplineComponent* Spline;

	/// Scene component that acts as the movable root for the boat mesh
	UPROPERTY()
	USceneComponent* RowBoatBase;

	/// Event handler called when the connected ship begins falling
	UFUNCTION()
	void HandleShipBeginsFall(float ShipFallSpeed);

	/// Tracks how far along the spline the boat has traveled
	float DistanceAlongSpline = 0.f;

	/// Whether the boat is currently falling
	bool bFalling = false;

	/// Speed at which the boat falls downward
	float FallSpeed = 0;
	
	/// The length of the spline the boat is moving along
	float SplineLength = 0;
};
