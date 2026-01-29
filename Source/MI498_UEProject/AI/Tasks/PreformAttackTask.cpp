#include "PreformAttackTask.h"
#include "StateTreeExecutionContext.h"


const UStruct* FPreformAttackTask::GetInstanceDataType() const
{
	return FPreformAttackTaskInstanceData::StaticStruct(); 
}

EStateTreeRunStatus FPreformAttackTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	// Get the instance data for this task
	const FPreformAttackTaskInstanceData& Data = Context.GetInstanceData(*this);

	if (!Data.Actor || !Data.Target)
	{
		return EStateTreeRunStatus::Failed;
	}
	// TODO: Create a different task for set foucs!!
	Data.AIController->SetFocus(Data.Target);
	Data.Actor->Attack(Data.Target);
	return EStateTreeRunStatus::Succeeded;
}
