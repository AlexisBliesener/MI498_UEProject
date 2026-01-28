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
		UE_LOG(LogTemp, Warning, TEXT("Actor is null"));
		return EStateTreeRunStatus::Failed;
	}

	UWorld* World = Data.Actor->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is null"));
		return EStateTreeRunStatus::Failed;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSys)
	{
		UE_LOG(LogTemp, Warning, TEXT("NavSys is null"));
		return EStateTreeRunStatus::Failed;
	}

	FVector Origin = Data.Actor->GetActorLocation();
	FNavLocation NavLocation;

	const bool bFound = NavSys->GetRandomReachablePointInRadius(Origin, Data.SearchRadius, NavLocation);
	if (bFound)
	{
		Data.RandomLocation = NavLocation.Location;

		UE_LOG(LogTemp, Warning, TEXT("PMEW SUCCENS AAAAA SUCCES. Name: %ls Location: %s"), *Data.Actor->GetName() ,*Data.RandomLocation.ToString());
		return EStateTreeRunStatus::Succeeded;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find random location"));
		return EStateTreeRunStatus::Failed;
	}
}
