#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Characters/Enemies/SwingingEnemy/SwingingEnemy.h"
#include "UseSwingSplineTask.generated.h"


/**
 * StateTree instance data for the Use Swing Spline Task.
 */
USTRUCT(BlueprintType,meta = (DisplayName = "Use Swing Spline Task"))
struct FUseSwingSplineTaskInstanceData
{
	GENERATED_BODY()

	/**
	 * Points to the enemy character being controlled by the AI.
	 */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<ASwingingEnemy> Actor;

	/**
	 * Points to the AI controller responsible for controlling the enemy actor.
	 */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyAIController> AIController;
};

/**
 * This task is being used in the State Tree
 * it sends an event to the state tree to transition from state to another state
 */
USTRUCT(BlueprintType)
struct MI498_UEPROJECT_API FUseSwingSplineTask : public FStateTreeTaskCommonBase
{
public:
	GENERATED_BODY()
	using FInstanceDataType = FUseSwingSplineTaskInstanceData;

	/**
	 * Gets the type of instance data used by this task.
	 * @return The UStruct representing the instance data type.
	 */
	virtual const UStruct* GetInstanceDataType() const override;
	/**
	 * Executes the EnterState logic for the Send Event Task.
	 * Handles sending a state tree event to the AI component if a valid event tag is provided.
	 *
	 * @param Context The execution context providing necessary runtime data.
	 * @param Transition Contains data about the state transition.
	 * @return The status of the task after execution.
	 */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
	                                       const FStateTreeTransitionResult& Transition) const override;
	
	
};
