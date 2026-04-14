#include "ProbabilityAttackTask.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"



const UStruct* FProbabilityAttackTask::GetInstanceDataType() const
{
	return FProbabilityAttackTaskInstanceData::StaticStruct(); 
}

EStateTreeRunStatus FProbabilityAttackTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FProbabilityAttackTaskInstanceData& Data = Context.GetInstanceData(*this);

	if (!Data.Actor || !Data.Target)
	{
		return EStateTreeRunStatus::Failed;
	}

	float randomRoll = FMath::FRand(); // Returns 0.0 to 1.0
	if (randomRoll > Data.ChanceToAttack)
	{
		return EStateTreeRunStatus::Failed;
	}

	// Success: Perform the attack
	//Data.AIController->SetFocus(Data.Target);
	Data.Actor->Attack(Data.Target, false); 
    
	//Data.AIController->ClearFocus(EAssetTypeCategories::Gameplay);
	return EStateTreeRunStatus::Succeeded;
}


