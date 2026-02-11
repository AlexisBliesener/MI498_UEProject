// Fill out your copyright notice in the Description page of Project Settings.


#include "SwingingEnemy.h"

#include "EngineUtils.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/AI/SwingingEnemy/SwingSpline.h"
#include "MI498_UEProject/AI/SwingingEnemy/SwingSplineManager.h"



// Sets default values
ASwingingEnemy::ASwingingEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


// Called when the game starts or when spawned
void ASwingingEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentSpline = nullptr;
	SplineDistance = 0.f;
	// Get the manager component only once in the begin play... 
	for (TActorIterator<ASwingSplineManager> It(GetWorld()); It; ++It)
	{
		Manager = *It;
		break;
	}
}

void ASwingingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Manager && bDebug)
	{
		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),
			MaxMoveToPointRadius,
			32,
			FColor::Blue,
			false,
			0.f,
			0,
			1.f
		);
		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),
			MaxReservePointRadius,
			32,
			FColor::Blue,
			false,
			0.f,
			0,
			1.f
		);
		
		const FVector enemyLoc = GetActorLocation();


		DrawDebugCircle(
			GetWorld(),
			enemyLoc,
			XYDetectionRadius,
			64,
			FColor::Green,
			false,
			0.f,
			0,
			2.f,
			FVector(1,0,0),  
			FVector(0,1,0),  
			false
		);
		
		
		const FVector top = enemyLoc;
		const FVector bottom = enemyLoc - FVector(0.f, 0.f, UnderEnemyTolerance);

		DrawDebugLine(
			GetWorld(),
			top,
			bottom,
			FColor::Red,
			false,
			0.f,
			0,
			3.f
		);

		DrawDebugSphere(
			GetWorld(),
			bottom,
			20.f,
			12,
			FColor::Red,
			false,
			0.f
		);
	}

	if (!bIsSwinging || !CurrentSpline) return;

	SplineDistance += SwingSpeed * DeltaTime;

	const float length = CurrentSpline->Spline->GetSplineLength();
	SplineDistance = FMath::Clamp(SplineDistance, 0.f, length);

	FVector goalPos = CurrentSpline->Spline->GetLocationAtDistanceAlongSpline(SplineDistance,ESplineCoordinateSpace::World);

	SetActorLocation(goalPos, false);
}

bool ASwingingEnemy::GetASwingPoint()
{
	if (!Manager)
	{
		UE_LOG(EnemyAILog, Error, TEXT("No Swing Spline Manager found on %s"), *GetName());
		return false;
	}
	
	CurrentSpline = Manager->RequestClosestAvailableSpline(this,GetActorLocation(), false);

	SplineDistance = 0.f;
	
	return CurrentSpline != nullptr;
}

