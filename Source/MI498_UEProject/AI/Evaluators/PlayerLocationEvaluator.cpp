// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerLocationEvaluator.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/AI/EnemyAIController.h"

void UPlayerLocationEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	StunnedTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Event.Stunned"));
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
	
	if (!IsValid(Actor) || !IsValid(AIController))
	{
		return;
	}

	if (IsValid(AIController->AcquiredTarget))
	{
		if (!IsValid(TargetActor))
		{
			TargetActor = AIController->AcquiredTarget;
		}
	}else
	{
		if (!IsValid(TargetActor))
		{
			TargetActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		}
	}

	if (!IsValid(TargetActor))
	{
		return;
	}
	const FVector enemyLoc = Actor->GetActorLocation();
	const FVector targetLoc = TargetActor->GetActorLocation();
	
	const float distSquared = FVector::DistSquared2D(enemyLoc, targetLoc);
	const float distZ = FMath::Abs(targetLoc.Z - enemyLoc.Z);

	bTargetInBombRange = distSquared <= FMath::Square(Actor->AttackStartDistance);
	bTargetChaseRange = distSquared <= FMath::Square(Actor->ChaseRange);
    
	// melee range
	bTargetInMeleeRange = (distSquared <= FMath::Square(Actor->MeleeRange)) && (distZ <= Actor->MeleeZTolerance);

	if (Actor->CurrentTags.HasTag(StunnedTag))
	{
		LastEvent = SwingingEnemyEnums::Idle;
		return;
	}
	
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
	//else if (LastEvent != SwingingEnemyEnums::Swing)
	//{
	//	AIController->GetStateTreeAIComponent()->SendStateTreeEvent(FGameplayTag::RequestGameplayTag(FName("StateTree.SwingingEnemy.Swinging")));
	//	LastEvent = SwingingEnemyEnums::Swing;

	//}
}
