#include "JumpFromSwing.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"




const UStruct* FJumpFromSwing::GetInstanceDataType() const
{
	return FJumpFromSwingInstanceData::StaticStruct(); 
}

EStateTreeRunStatus FJumpFromSwing::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FJumpFromSwingInstanceData& Data = Context.GetInstanceData(*this);
	
	if (!Data.Actor)
	{
		return EStateTreeRunStatus::Failed;	
	}
	
	Data.Actor->DetachAndJumpToGround( Data.Result);
	return EStateTreeRunStatus::Succeeded;
}
