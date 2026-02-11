#include "SetSwingSpeedTask.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"




const UStruct* FSetSwingSpeedTask::GetInstanceDataType() const
{
	return FSetSwingSpeedTaskInstanceData::StaticStruct(); 
}

EStateTreeRunStatus FSetSwingSpeedTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FSetSwingSpeedTaskInstanceData& Data = Context.GetInstanceData(*this);
	Data.Actor->SwingSpeed = Data.NewSwingSpeed;
	return EStateTreeRunStatus::Succeeded;
}
