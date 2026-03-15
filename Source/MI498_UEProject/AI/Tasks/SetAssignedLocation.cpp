#include "SetAssignedLocation.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "MI498_UEProject/AI/Systems/EnemyManagerSubsystem.h"


const UStruct* FSetAssignedLocation::GetInstanceDataType() const
{
	return FSetAssignedLocationInstanceData::StaticStruct(); 
}

EStateTreeRunStatus FSetAssignedLocation::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FSetAssignedLocationInstanceData& data = Context.GetInstanceData(*this);
	
	if (!data.Actor)
	{
		UE_LOG(EnemyAILog, Warning, TEXT("Actor is null"));
		return EStateTreeRunStatus::Failed;
	}
	
	UWorld* world = data.Actor->GetWorld();
	if (!world)
	{
		UE_LOG(EnemyAILog, Warning, TEXT("World is null"));
		return EStateTreeRunStatus::Failed;
	}
	
	data.Actor->AssignedLocation = data.AssignedLocation;

	if (UEnemyManagerSubsystem* enemySubsystem = world->GetSubsystem<UEnemyManagerSubsystem>())
	{
		// zero = they're not taking this location rn 
		if (data.AssignedLocation.IsNearlyZero())
		{
			enemySubsystem->EnemiesAroundPlayer.Remove(data.Actor);
		}else
		{
			enemySubsystem->EnemiesAroundPlayer.Add(data.Actor);
		}
	}else
	{
		UE_LOG(EnemyAILog, Warning, TEXT("enemySubsystem is null"));
		return EStateTreeRunStatus::Failed;
	}
	return EStateTreeRunStatus::Succeeded;
}


