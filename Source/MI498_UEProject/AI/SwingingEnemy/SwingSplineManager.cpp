// Fill out your copyright notice in the Description page of Project Settings.


#include "SwingSplineManager.h"

#include "NavigationSystem.h"
#include "MI498_UEProject/Characters/Enemies/SwingingEnemy/SwingingEnemy.h"


// Sets default values
ASwingSplineManager::ASwingSplineManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));;
}

ASwingSpline* ASwingSplineManager::RequestClosestAvailableSpline(ASwingingEnemy* Requester,const FVector& FromLocation, const bool bIsReserve)
{
	if (!Requester)
		return nullptr;

	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());

	if (!navSys)
		return nullptr;

	ASwingSpline* bestSpline = nullptr;
	// set max destination for each one 
	// the reservation distance is different than the move to point 
	const float maxDistSq = FMath::Square(bIsReserve ? Requester->MaxReservePointRadius : Requester->MaxMoveToPointRadius);
	float bestDistSq = maxDistSq;
	const FVector queryExtent(1000, 1000.f, 1000.f);
	for (ASwingSpline* spline : Splines)
	{
		// skip the spline if is already taken...  
		if (!IsValid(spline) || !spline->IsAvailable(Requester))
			continue;

		FNavLocation projectedLoc;
		if (!navSys->ProjectPointToNavigation(
				spline->GetStartLocation(),
				projectedLoc,
				queryExtent))
		{
			// this means the spline is not reachable (in nav mesh) 
			continue;
		}
		

		const float distSq = FVector::DistSquared(FromLocation, projectedLoc.Location);
		
		if (distSq > maxDistSq)
		{
			continue;
		}
		if (distSq < bestDistSq)
		{
			bestDistSq = distSq;
			bestSpline = spline;
		}
	}

	if (bestSpline)
	{
		bestSpline->UseSpline(Requester);
	}

	return bestSpline;
}
