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

protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

private:
	/// Reference to the spline component that defines the boat's movement path
	UPROPERTY()
	USplineComponent* Spline;

	/// Scene component that acts as the movable root for the boat mesh
	UPROPERTY()
	USceneComponent* RowBoatBase;

	/// Tracks how far along the spline the boat has traveled
	float DistanceAlongSpline = 0.f;
	
	/// The length of the spline the boat is moving along
	float SplineLength = 0;
};
