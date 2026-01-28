#include "SendEventTask.h"
#include "NavigationSystem.h"
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
		AEnemyAIController* aiController = Cast<AEnemyAIController>(Context.GetOwner());
		if (aiController)
		{
			if (UStateTreeAIComponent* StateTreeComp = aiController->FindComponentByClass<UStateTreeAIComponent>())
			{
				FStateTreeEvent Event(Data.EventTag);
				StateTreeComp->SendStateTreeEvent(Event);
			}
		}
	}

	return EStateTreeRunStatus::Succeeded;
}
