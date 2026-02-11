// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerLocationEvaluator.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/AI/EnemyAIController.h"

void UPlayerLocationEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

}

void UPlayerLocationEvaluator::TreeStop(FStateTreeExecutionContext& Context)
{
	Super::TreeStop(Context);
	
}

void UPlayerLocationEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	UWorld* World = Actor->GetWorld();
	if (!World)
	{
		bPlayerUnderEnemy = false;
		bPlayerInXYRange = false;
		bPlayerInMeleeRange = false;
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	if (!PlayerPawn)
	{
		bPlayerUnderEnemy = false;
		bPlayerInXYRange = false;
		bPlayerInMeleeRange = false;
		return;
	}
	// cache the player
	Player = PlayerPawn;
	bPlayerInXYRange = false;
	bPlayerUnderEnemy = false;
	bPlayerInMeleeRange = false;
	if (!IsValid(Actor) || !IsValid(Player))
	{
		UE_LOG(EnemyAILog, Error, TEXT("Actor or player not found on Player Location Evaluator! "))
		return;
	}

	const FVector EnemyLoc = Actor->GetActorLocation();
	const FVector PlayerLoc = Player->GetActorLocation();
	
	const FVector EnemyXY(EnemyLoc.X, EnemyLoc.Y, 0.f); // ignore z axis 
	const FVector PlayerXY(PlayerLoc.X, PlayerLoc.Y, 0.f);   // ignore z axis 

	const float DistSqXY = FVector::DistSquared(EnemyXY, PlayerXY);

	bPlayerInXYRange = DistSqXY <= FMath::Square(Actor->XYDetectionRadius);
	
	const float DistZ = FMath::Abs(PlayerLoc.Z - EnemyLoc.Z);
	// Player under enemy? (vertical check only!)
	bPlayerUnderEnemy = bPlayerInXYRange && (PlayerLoc.Z < EnemyLoc.Z) &&
		FMath::Abs(PlayerLoc.Z - EnemyLoc.Z) <= Actor->UnderEnemyTolerance;
		
	// check for melee range with a tolerance for the height 
	bPlayerInMeleeRange = (DistSqXY <= FMath::Square(Actor->MeleeRange)) &&
					  (DistZ <= Actor->MeleeZTolerance);
}
