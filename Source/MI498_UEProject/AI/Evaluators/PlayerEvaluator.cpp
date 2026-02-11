// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerEvaluator.h"
#include "Kismet/GameplayStatics.h"

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
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	if (!PlayerPawn)
	{
		DistanceToPlayer = 0.0f;
		return;
	}
	// cache the player
	Player = PlayerPawn;
	if (!IsValid(Player))
	{
		return;
	}
	// calculate the distance to the player
	DistanceToPlayer = FVector::Dist(Actor->GetActorLocation(), Player->GetActorLocation());
	
	
	// change if the player is in range for attack
	bIsPlayerInRange = (DistanceToPlayer <= AttackRange);
}
