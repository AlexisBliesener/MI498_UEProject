#include "BackToSwing.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"





const UStruct* FBackToSwing::GetInstanceDataType() const
{
	return FBackToSwingInstanceData::StaticStruct(); 
}

EStateTreeRunStatus FBackToSwing::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FBackToSwingInstanceData& Data = Context.GetInstanceData(*this);
	if (!Data.Actor)
	{
		return EStateTreeRunStatus::Failed;	
	}
	
	Data.Actor->ShootRopeAndSwing();
	return EStateTreeRunStatus::Succeeded;
}
