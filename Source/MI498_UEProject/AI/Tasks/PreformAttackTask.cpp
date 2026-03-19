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

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FPreformAttackTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FPreformAttacksTaskInstanceData& Data = Context.GetInstanceData(*this);
	if (!Data.Actor || !Data.Target)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	// TODO: Create a different task for set foucs!!
	Data.AIController->SetFocus(Data.Target, EAIFocusPriority::Gameplay);
	Data.Actor->Attack(Data.Target, Data.bIsSecondaryAttack);
	
	if (!Data.bRunForever)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	return EStateTreeRunStatus::Running;
}
