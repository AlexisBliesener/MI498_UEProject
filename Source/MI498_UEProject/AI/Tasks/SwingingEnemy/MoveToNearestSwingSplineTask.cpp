#include "MoveToNearestSwingSplineTask.h"

#include "NavigationSystem.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MI498_UEProject/Characters/Enemies/SwingingEnemy/SwingingEnemy.h"
#include "MI498_UEProject/AI/SwingingEnemy/SwingSplineManager.h"
#include "Navigation/PathFollowingComponent.h"



EStateTreeRunStatus FMoveToNearestSwingSplineTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FMoveToNearestSwingSplineTaskInstanceData& Data = Context.GetInstanceData(*this);
	
	if (!IsValid(Data.AIController))
		return EStateTreeRunStatus::Failed;

	switch (Data.AIController->GetMoveStatus())
	{
	case EPathFollowingStatus::Idle:
		return EStateTreeRunStatus::Succeeded;

	case EPathFollowingStatus::Moving:
		return EStateTreeRunStatus::Running;

	default:
		return EStateTreeRunStatus::Failed;
	}
}


const UStruct* FMoveToNearestSwingSplineTask::GetInstanceDataType() const
{
	return FMoveToNearestSwingSplineTaskInstanceData::StaticStruct(); 
}

EStateTreeRunStatus FMoveToNearestSwingSplineTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FMoveToNearestSwingSplineTaskInstanceData& Data = Context.GetInstanceData(*this);

	
	if (!IsValid(Data.Actor) || !IsValid(Data.Actor->Manager))
		return EStateTreeRunStatus::Failed;

	ASwingSpline* targetSpline = Data.Actor->Manager->RequestClosestAvailableSpline(Data.Actor,Data.Actor->GetActorLocation(), true);

	if (!IsValid(targetSpline))
		return EStateTreeRunStatus::Failed;
	
	FAIMoveRequest moveReq;
	moveReq.SetAcceptanceRadius(10.f);
	FVector goalLocation = GetNearestNavLocation(Data.Actor->GetWorld(),targetSpline->GetStartLocation(), Data.Actor->MaxReservePointRadius);
	const EPathFollowingRequestResult::Type result = Data.AIController->MoveToLocation(goalLocation);
	Data.Actor->GetCharacterMovement()->SetMovementMode(MOVE_NavWalking);
	return result == EPathFollowingRequestResult::RequestSuccessful
		? EStateTreeRunStatus::Running
		: EStateTreeRunStatus::Failed;
}

FVector FMoveToNearestSwingSplineTask::GetNearestNavLocation(UWorld* World, const FVector& DesiredLocation, const float& MaxReservePointRadius)
{
	if (!World)
	{
		return DesiredLocation;
	}

	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(World);
	if (!navSys)
	{
		return DesiredLocation;
	}

	FNavLocation projectedLocation;
	const FVector queryExtent(MaxReservePointRadius, MaxReservePointRadius, 1000.f);

	if (const bool bFound = navSys->ProjectPointToNavigation(DesiredLocation,projectedLocation,queryExtent))
	{
		return projectedLocation.Location;
	}

	return DesiredLocation;
}

