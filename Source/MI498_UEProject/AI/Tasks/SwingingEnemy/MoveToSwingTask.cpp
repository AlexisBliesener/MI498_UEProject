#include "MoveToSwingTask.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "MI498_UEProject/Characters/Enemies/SwingingEnemy/SwingingEnemy.h"
#include "MI498_UEProject/AI/SwingingEnemy/SwingSplineManager.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


EStateTreeRunStatus FMoveToSwingTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FMoveToSwingTaskInstanceData& Data = Context.GetInstanceData(*this);
	ASwingingEnemy* swingingEnemy = Data.Actor;
	USplineComponent* spline = swingingEnemy->CurrentSpline->Spline;
	const float length = spline->GetSplineLength();
	
	if (swingingEnemy->SplineDistance >= length)
	{
		
		return EStateTreeRunStatus::Succeeded;
	}
	
	
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FMoveToSwingTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FMoveToSwingTaskInstanceData& Data = Context.GetInstanceData(*this);
	if (!IsValid(Data.Actor) || !IsValid(Data.Actor->Manager) || !IsValid(Data.Actor->CurrentSpline))
	{
		UE_LOG(EnemyAILog, Error, TEXT("No CurrentSpline found or manager or enemy found on MoveToSwing Task! for %s"), *Data.Actor->GetName());
		return EStateTreeRunStatus::Failed;
	}
	// if there is no spline the task will fail because obviously how would you move without a path?
	if (Data.Actor->CurrentSpline == nullptr)
	{
		UE_LOG(EnemyAILog, Error, TEXT("You tried to move an enemy without spline!! for %s"), *Data.Actor->GetName());
		return EStateTreeRunStatus::Failed;
	}
	Data.Actor->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	Data.Actor->SplineDistance = 0.f;
	Data.Actor->bIsSwinging = true;
	return EStateTreeRunStatus::Running;
}

void FMoveToSwingTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FMoveToSwingTaskInstanceData& Data = Context.GetInstanceData(*this);
	// call the swinging enemy to stop swing (please) 
	Data.Actor->bIsSwinging = false;
	Data.Actor->SplineDistance = 0.f;
	Data.Actor->GetCharacterMovement()->SetMovementMode(MOVE_NavWalking);
}


const UStruct* FMoveToSwingTask::GetInstanceDataType() const
{
	return FMoveToSwingTaskInstanceData::StaticStruct(); 
}
