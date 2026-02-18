#include "WaitOnSwingStopTask.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "MI498_UEProject/AI/SwingingEnemy/SwingSpline.h"


FWaitOnSwingStopTask::FWaitOnSwingStopTask()
{
	bConsideredForScheduling = false;
	bShouldCopyBoundPropertiesOnTick = false;
	bShouldCopyBoundPropertiesOnExitState = false;
}
EStateTreeRunStatus FWaitOnSwingStopTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.RemainingTime -= DeltaTime;

	if (InstanceData.RemainingTime <= 0.f)
	{
		Context.RemoveScheduledTickRequest(InstanceData.ScheduledTickHandle);
		return EStateTreeRunStatus::Succeeded;
	}
	Context.UpdateScheduledTickRequest(InstanceData.ScheduledTickHandle, FStateTreeScheduledTick::MakeCustomTickRate(InstanceData.RemainingTime));
	return EStateTreeRunStatus::Running;
}


const UStruct* FWaitOnSwingStopTask::GetInstanceDataType() const
{
	return FWaitOnSwingStopTaskInstanceData::StaticStruct(); 
}

EStateTreeRunStatus FWaitOnSwingStopTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	// if there is no stop time or there is no spline, it will make the task as completed 
	if (!IsValid(InstanceData.Actor->CurrentSpline) || InstanceData.Actor->CurrentSpline->StopTime <= 0.f)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	InstanceData.RemainingTime = InstanceData.Actor->CurrentSpline->StopTime;
	InstanceData.ScheduledTickHandle = Context.AddScheduledTickRequest(FStateTreeScheduledTick::MakeCustomTickRate(InstanceData.RemainingTime));
	return EStateTreeRunStatus::Running;
}

void FWaitOnSwingStopTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	Context.RemoveScheduledTickRequest(InstanceData.ScheduledTickHandle);
}

