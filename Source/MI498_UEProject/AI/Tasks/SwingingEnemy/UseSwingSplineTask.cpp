#include "UseSwingSplineTask.h"
#include "StateTreeLinker.h"
#include "StateTreeExecutionContext.h"
#include "MI498_UEProject/AI/SwingingEnemy/SwingSplineManager.h"


const UStruct* FUseSwingSplineTask::GetInstanceDataType() const
{
	return FUseSwingSplineTaskInstanceData::StaticStruct(); 
}

EStateTreeRunStatus FUseSwingSplineTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FUseSwingSplineTaskInstanceData& Data = Context.GetInstanceData(*this);
	if (!IsValid(Data.Actor) || !IsValid(Data.Actor->Manager))
	{
		UE_LOG(EnemyAILog, Error, TEXT("Actor or manager not found on UseSwingSplineTask!!"));
		return EStateTreeRunStatus::Failed;
	}



	return Data.Actor->GetASwingPoint()
		? EStateTreeRunStatus::Succeeded
		: EStateTreeRunStatus::Failed;
}
