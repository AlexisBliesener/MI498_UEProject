#include "RowBoat.h"
#include "Ship.h"

ARowBoat::ARowBoat()
{
	PrimaryActorTick.bCanEverTick = true;

	/// Create a root component for the actor
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	/// Create a scene component that will represent the boat base
	RowBoatBase = CreateDefaultSubobject<USceneComponent>(TEXT("BoatRoot"));
	RowBoatBase->SetupAttachment(RootComponent);
}

void ARowBoat::BeginPlay()
{
	Super::BeginPlay();

	/// Try to find a spline component that exists on this actor 
	Spline = FindComponentByClass<USplineComponent>();

	if (!Spline)
	{
		UE_LOG(LogTemp, Warning, TEXT("RowBoat: No spline component found."));
	}
	
	/// Get spline length
	SplineLength = Spline->GetSplineLength();
}

void ARowBoat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/// Ensure both spline and rowboatbase are assigned
	if (Spline && RowBoatBase)
	{
		/// Move the boat along the spline based on speed
		DistanceAlongSpline += Speed * DeltaTime;

		/// Loop the movement so the boat keeps circling the spline
		DistanceAlongSpline = FMath::Fmod(DistanceAlongSpline, SplineLength);

		/// Get the world location along the spline at the current distance
		FVector NewLocation = Spline->GetLocationAtDistanceAlongSpline(
			DistanceAlongSpline,
			ESplineCoordinateSpace::World
		);

		/// Get the direction the spline is pointing at that position
		FVector Direction = Spline->GetDirectionAtDistanceAlongSpline(
			DistanceAlongSpline,
			ESplineCoordinateSpace::World
		);

		/// Convert direction to rotation so the boat faces the movement direction
		FRotator LookRotation = Direction.Rotation();

		/// Prevent the boat from tilting up/down
		LookRotation.Pitch = 0;

		/// Adjust yaw so the boat mesh faces forward correctly
		LookRotation.Yaw += 90.f;

		/// Apply position, rotation to the boat base
		RowBoatBase->SetWorldLocationAndRotationNoPhysics(NewLocation, LookRotation);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Spline or row boat base is null"));
	}
}