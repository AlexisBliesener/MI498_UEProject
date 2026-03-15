// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_ProjectedQuerier.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "MI498_UEProject/AI/EnemyAIController.h"

void UEnvQueryContext_ProjectedQuerier::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	AActor* querierActor = Cast<AActor>(QueryInstance.Owner.Get());
	if (!querierActor) return;

	AEnemyBase* enemy = Cast<AEnemyBase>(querierActor);
	// if the enemy is still empty that means the querier is the ai controller 
	if (enemy == nullptr) 
	{
		if (AController* controller = Cast<AController>(querierActor))
		{
			enemy = Cast<AEnemyBase>(controller->GetPawn());
		}
	}

	if (enemy != nullptr)
	{
		if (enemy->RealShip && enemy->HiddenShip)
		{
			FVector localPos = enemy->RealShip->GetActorTransform().InverseTransformPosition(enemy->GetActorLocation());
			FVector hiddenLocation = enemy->HiddenShip->GetActorTransform().TransformPosition(localPos);
			UEnvQueryItemType_Point::SetContextHelper(ContextData, hiddenLocation);
		}
		else
		{
			UE_LOG(EnemyAILog, Error, TEXT("Enemy doesn't have a ship assigned to it! (nav mesh won't work on this enemy!!)"));
		}
	}
	else
	{
		// THIS IS FOR DEBUG (unreal method..)
		UEnvQueryItemType_Point::SetContextHelper(ContextData, querierActor->GetActorLocation());
	}
}
