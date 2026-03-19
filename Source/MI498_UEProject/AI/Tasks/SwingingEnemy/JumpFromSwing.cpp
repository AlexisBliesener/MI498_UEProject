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
	FVector realPos = Data.Result;

	if (Data.Actor->RealShip && Data.Actor->HiddenShip)
	{
		float distToFake = FVector::DistSquared(realPos, Data.Actor->HiddenShip->GetActorLocation());
		float distToReal = FVector::DistSquared(realPos, Data.Actor->RealShip->GetActorLocation());

		// if the point is closer on the fake ship then translate it to the real ship!
		if (distToFake < distToReal)
		{
			FVector localPos = Data.Actor->HiddenShip->GetActorTransform().InverseTransformPosition(realPos);
			realPos = Data.Actor->RealShip->GetActorTransform().TransformPosition(localPos);
		}
	}
	
	Data.Actor->DetachAndJumpToGround( realPos);
	return EStateTreeRunStatus::Succeeded;
}
