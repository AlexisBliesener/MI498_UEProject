#include "Ship.h"
#include "GameFramework/Actor.h"

AShip::AShip()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShip::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bFalling)
	{
		Fall(DeltaSeconds);
	}
}

void AShip::Fall(const float DeltaTime) 
{
	FVector FallOffset = FVector(0.f, 0.f, -FallSpeed * DeltaTime);
	AddActorWorldOffset(FallOffset, true);
}

