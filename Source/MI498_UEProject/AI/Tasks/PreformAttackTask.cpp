#include "PreformAttackTask.h"
#include "StateTreeExecutionContext.h"

const UStruct* FPreformAttackTask::GetInstanceDataType() const
{
	return FPreformAttacksTaskInstanceData::StaticStruct(); 
}

EStateTreeRunStatus FPreformAttackTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	
	// Get the instance data for this task
	const FPreformAttacksTaskInstanceData& Data = Context.GetInstanceData(*this);
	if (Data.Actor && Data.Target && Data.AIController)
	{
		Data.AIController->SetFocus(Data.Target);
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FPreformAttackTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FPreformAttacksTaskInstanceData& Data = Context.GetInstanceData(*this);
	if (!Data.Actor || !Data.Target)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	if (!Data.Actor->IsFacingPlayer(Data.Target, 0.90f))
	{
		return EStateTreeRunStatus::Running; 
	}
	
	if (!Data.Actor->bIsAttacking)
	{
		Data.Actor->Attack(Data.Target, Data.bIsSecondaryAttack);
	}
	
	if (Data.bWaitUntilAttackIsFinished && Data.Actor->bIsAttacking)
	{
		return EStateTreeRunStatus::Running;
	}
	
	if (Data.bRunForever)
	{
		if (Data.Actor->GetCanShoot() && !Data.Actor->bIsAttacking)
		{
			Data.Actor->Attack(Data.Target, Data.bIsSecondaryAttack);
		}
		return EStateTreeRunStatus::Running;
	}
	
	return EStateTreeRunStatus::Succeeded;
}

void FPreformAttackTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FPreformAttacksTaskInstanceData& data = Context.GetInstanceData(*this);
	if (data.AIController)
	{
		data.AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
}
