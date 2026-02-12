#include "ReleaseSwingSplineTask.h"
#include "StateTreeExecutionContext.h"
#include "MI498_UEProject/AI/SwingingEnemy/SwingSplineManager.h"
#include "StateTreeExecutionTypes.h"




const UStruct* FReleaseSwingSplineTask::GetInstanceDataType() const
{
	return FReleaseSwingSplineTaskInstanceData::StaticStruct(); 
}

EStateTreeRunStatus FReleaseSwingSplineTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FReleaseSwingSplineTaskInstanceData& Data = Context.GetInstanceData(*this);
	
	Data.Actor->CurrentSpline->ReleaseSpline(Data.Actor);
	
	// if there IS still a spline attached to this enemy then that task has failed to release the spline! 
	return IsValid(Data.Actor->CurrentSpline)
		? EStateTreeRunStatus::Failed
		: EStateTreeRunStatus::Succeeded;
}