#include "MoveToCustomTask.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"

bool FMoveToCustomTask::Link(FStateTreeLinker& Linker)
{
	// we should call Super::Link first because in their function they set bShouldCallTick and bShouldCopyBoundPropertiesOnTick to false
	bool bResult = Super::Link(Linker);
	// The unreal's move to class is turned it off so we need to turn it on here, so the tick can tick and we can update the target location...
	bShouldCallTick = true;
	bShouldCopyBoundPropertiesOnTick = true;
    
	return bResult;
}

EStateTreeRunStatus FMoveToCustomTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& instanceData = Context.GetInstanceData(*this);
    
	if (instanceData.MoveToTask)
	{
		if (instanceData.AIController)
		{
			if (AEnemyBase* enemy = Cast<AEnemyBase>(instanceData.AIController->GetPawn()))
			{

				if (enemy->IsJumping())
				{
					return EStateTreeRunStatus::Running;
				}
			}
		}
		if (instanceData.bTrackMovingGoal && instanceData.TargetActor)
		{
			// get the player location right now
			const FVector currentPlayerPos = instanceData.TargetActor->GetActorLocation();
          
			// see if player move away more than the tolerance we set (same as unreal's one...) 
			if (FVector::DistSquared(currentPlayerPos, instanceData.Destination) > (instanceData.DestinationMoveTolerance * instanceData.DestinationMoveTolerance))
			{
				// update the new player location
				instanceData.Destination = currentPlayerPos;
				return PerformMoveTask(Context, *instanceData.AIController);
			}
		}
		return EStateTreeRunStatus::Running;
	}
	return EStateTreeRunStatus::Failed;
}
