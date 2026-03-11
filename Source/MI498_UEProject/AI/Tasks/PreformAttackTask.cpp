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

	if (!Data.Actor || !Data.Target)
	{
		return EStateTreeRunStatus::Failed;
	}
	// TODO: Create a different task for set foucs!!
	Data.AIController->SetFocus(Data.Target);
	Data.Actor->Attack(Data.Target, Data.bIsSecondaryAttack);
	return EStateTreeRunStatus::Succeeded;
}
