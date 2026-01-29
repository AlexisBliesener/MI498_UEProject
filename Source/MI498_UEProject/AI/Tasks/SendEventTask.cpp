#include "SendEventTask.h"
#include "StateTreeExecutionContext.h"


const UStruct* FSendEventTask::GetInstanceDataType() const
{
	return FSendEventTaskInstanceData::StaticStruct(); 
}

EStateTreeRunStatus FSendEventTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FSendEventTaskInstanceData& Data = Context.GetInstanceData(*this);

	if (Data.EventTag.IsValid())
	{
		if (AEnemyAIController* aiController = Cast<AEnemyAIController>(Context.GetOwner()))
		{
			if (IsValid(aiController->GetStateTreeAIComponent()))
			{
				FStateTreeEvent Event(Data.EventTag);
				aiController->GetStateTreeAIComponent()->SendStateTreeEvent(Event);
			}
		}
	}

	return EStateTreeRunStatus::Succeeded;
}
