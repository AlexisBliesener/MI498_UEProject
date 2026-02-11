#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MoveToSwingTask.generated.h"


class ASwingingEnemy;
/**
 * Holds data for the swing point Task.
 */
USTRUCT(BlueprintType,meta = (DisplayName = "Move to swing point "))
struct FMoveToSwingTaskInstanceData
{
	GENERATED_BODY()
	/**
	 * Pointer to an enemy actor instance.
	 */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<ASwingingEnemy> Actor;
};

/**
 * This task is being used in the State Tree
 * it gets a random point around the enemy and return it 
 */
USTRUCT(BlueprintType)
struct MI498_UEPROJECT_API FMoveToSwingTask : public FStateTreeTaskCommonBase
{
public:
	GENERATED_BODY()
	using FInstanceDataType = FMoveToSwingTaskInstanceData;

	/**
	 * Executes the task logic during the Tick phase of the StateTree.
	 * This function is called periodically to update the task behavior as part
	 * of the StateTree's execution loop.
	 *
	 * @param Context The execution context providing necessary runtime data and state.
	 * @param DeltaTime The amount of time that has elapsed since the last Tick call.
	 * @return The status of the task after the Tick operation.
	 */
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	/**
	 * Executes the EnterState logic for the task when a StateTree enters this state.
	 * This function is invoked to initialize the state and prepare it for execution.
	 *
	 * @param Context The execution context providing necessary runtime data and state information.
	 * @param Transition Contains details about the state transition that triggered the EnterState call.
	 * @return The status of the task after the EnterState operation.
	 */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
										   const FStateTreeTransitionResult& Transition) const override;

	/**
	 * Executes the ExitState logic for the task.
	 * Performs necessary cleanup or state updates when a StateTree exits this task.
	 *
	 * @param Context The execution context providing necessary runtime data.
	 * @param Transition Contains information about the state transition that triggered the ExitState call.
	 */
	virtual void ExitState(FStateTreeExecutionContext& Context,
						   const FStateTreeTransitionResult& Transition) const override;

protected:
	/**
	 * Gets the type of instance data used by this task.
	 * @return The UStruct representing the instance data type.
	 */
	virtual const UStruct* GetInstanceDataType() const override;
	
};
