// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerEvaluator.h"
#include "MI498_UEProject/AI/EnemyAIController.h"

void UPlayerEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

}

void UPlayerEvaluator::TreeStop(FStateTreeExecutionContext& Context)
{
	Super::TreeStop(Context);
	
}

void UPlayerEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	UWorld* World = Actor->GetWorld();
	if (!World)
	{
		DistanceToPlayer = 0.0f;
		bIsPlayerInRange = false;
		return;
	}
	if (!IsValid(Actor) || !IsValid(AIController))
	{
		bIsPlayerInRange = false;
		return;
	}

	AActor* playerPawn = AIController->AcquiredTarget;
	if (!IsValid(playerPawn))
	{
		DistanceToPlayer = 0.0f;
		bIsPlayerInRange = false;
		return;
	}

	// calculate the distance to the player
	DistanceToPlayer = FVector::Dist(Actor->GetActorLocation(), playerPawn->GetActorLocation());
	
	
	// change if the player is in range for attack
	bIsPlayerInRange = (DistanceToPlayer <= AttackRange);
}
