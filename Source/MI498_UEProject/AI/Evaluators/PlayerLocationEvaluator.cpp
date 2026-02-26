// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerLocationEvaluator.h"
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
	bTargetInBombRange = false;
	bTargetChaseRange = false;
	bTargetInMeleeRange = false;
	TargetActor = nullptr;
	
	if (!IsValid(Actor) || !IsValid(AIController))
	{
		return;
	}

	AActor* currentTarget = AIController->AcquiredTarget;
	if (!IsValid(currentTarget))
	{
		return;
	}
	TargetActor = currentTarget;

	const FVector enemyLoc = Actor->GetActorLocation();
	const FVector targetLoc = currentTarget->GetActorLocation();
	
	const float distSquared = FVector::DistSquared(enemyLoc, targetLoc);
	const float distZ = FMath::Abs(targetLoc.Z - enemyLoc.Z);

	bTargetInBombRange = distSquared <= FMath::Square(Actor->AttackStartDistance);
	bTargetChaseRange = distSquared <= FMath::Square(Actor->ChaseRange);
    
	// melee range
	bTargetInMeleeRange = (distSquared <= FMath::Square(Actor->MeleeRange)) && (distZ <= Actor->MeleeZTolerance);

	
	if (bTargetInMeleeRange && !(Actor->bIsSwinging))
	{
		if ( LastEvent == SwingingEnemyEnums::Melee)
		{
			return;
		}
       
		AIController->GetStateTreeAIComponent()->SendStateTreeEvent(FGameplayTag::RequestGameplayTag(FName("StateTree.SwingingEnemy.Melee")));
		LastEvent = SwingingEnemyEnums::Melee;


	}
	else if (bTargetChaseRange && !(Actor->bIsSwinging))
	{
		if ( LastEvent == SwingingEnemyEnums::Chase)
		{
			return;
		}
       
		AIController->GetStateTreeAIComponent()->SendStateTreeEvent(FGameplayTag::RequestGameplayTag(FName("StateTree.SwingingEnemy.Chase")));
		LastEvent = SwingingEnemyEnums::Chase;


	}
	else if (bTargetInBombRange)
	{
		if (LastEvent == SwingingEnemyEnums::ThrowABomb)
		{
			return;
		}
		AIController->GetStateTreeAIComponent()->SendStateTreeEvent(FGameplayTag::RequestGameplayTag(FName("StateTree.SwingingEnemy.ThrowABomb")));
		LastEvent = SwingingEnemyEnums::ThrowABomb;

	}
	else if (LastEvent != SwingingEnemyEnums::Swing)
	{
		AIController->GetStateTreeAIComponent()->SendStateTreeEvent(FGameplayTag::RequestGameplayTag(FName("StateTree.SwingingEnemy.Swinging")));
		LastEvent = SwingingEnemyEnums::Swing;

	}
}
