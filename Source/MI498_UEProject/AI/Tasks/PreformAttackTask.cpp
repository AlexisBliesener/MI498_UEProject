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
	if (Data.Actor && Data.Target)
	{
		Data.Actor->Attack(Data.Target, Data.bIsSecondaryAttack);
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
	
	Data.AIController->SetFocus(Data.Target, EAIFocusPriority::Gameplay);
	if (Data.bWaitUntilAttackIsFinished && Data.Actor->bIsAttacking)
	{
		return EStateTreeRunStatus::Running;
	}
	
	if (Data.bRunForever)
	{
		Data.Actor->Attack(Data.Target, Data.bIsSecondaryAttack);
		return EStateTreeRunStatus::Running;
	}
	
	return EStateTreeRunStatus::Succeeded;
}
