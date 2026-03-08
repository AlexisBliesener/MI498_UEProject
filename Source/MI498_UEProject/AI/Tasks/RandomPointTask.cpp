#include "RandomPointTask.h"
#include "NavigationSystem.h"
#include "StateTreeExecutionContext.h"


const UStruct* FRandomPointTask::GetInstanceDataType() const
{
	return FGetRandomLocationTaskInstanceData::StaticStruct(); 
}

EStateTreeRunStatus FRandomPointTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	// Get the instance data for this task
	FGetRandomLocationTaskInstanceData& Data = Context.GetInstanceData(*this);

	if (!Data.Actor)
	{
		UE_LOG(EnemyAILog, Warning, TEXT("Actor is null"));
		return EStateTreeRunStatus::Failed;
	}

	UWorld* World = Data.Actor->GetWorld();
	if (!World)
	{
		UE_LOG(EnemyAILog, Warning, TEXT("World is null"));
		return EStateTreeRunStatus::Failed;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSys)
	{
		UE_LOG(EnemyAILog, Warning, TEXT("NavSys is null"));
		return EStateTreeRunStatus::Failed;
	}

	if (Data.Actor->RealShip == nullptr)
	{
		UE_LOG(EnemyAILog, Warning, TEXT("RealShip is null"));
		return EStateTreeRunStatus::Failed;
	}
	if (Data.Actor->HiddenShip == nullptr)
	{
		UE_LOG(EnemyAILog, Warning, TEXT("HiddenShip is null"));
		return EStateTreeRunStatus::Failed;
	}
	FVector localOrigin = Data.Actor->RealShip->GetActorTransform().InverseTransformPosition(Data.Actor->EnemyInitLocation);
	FVector hiddenOrigin = Data.Actor->HiddenShip->GetActorTransform().TransformPosition(localOrigin);

	FNavLocation NavLocation;

	// Get a point from navmesh using the hidden ship 
	if (NavSys->GetRandomReachablePointInRadius(hiddenOrigin, Data.SearchRadius, NavLocation))
	{
		// translate that to the location on the real ship 
		FVector localResult = Data.Actor->HiddenShip->GetActorTransform().InverseTransformPosition(NavLocation.Location);
		FVector realResult = Data.Actor->RealShip->GetActorTransform().TransformPosition(localResult);

		// return the location to the state tree
		Data.RandomLocation = realResult; 
		return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Failed;
}
