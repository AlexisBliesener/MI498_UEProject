#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Characters/Enemies/SwingingEnemy/SwingingEnemy.h"
#include "ReleaseSwingSplineTask.generated.h"


/**
 * StateTree instance data for the Release Swing Spline Task.
 */
USTRUCT(BlueprintType,meta = (DisplayName = "Release Swing Spline Task"))
struct FReleaseSwingSplineTaskInstanceData
{
	GENERATED_BODY()

	/**
	 * Pointer to an enemy actor instance.
	 */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<ASwingingEnemy> Actor;

	/**
	 * Pointer to the AI controller for an enemy.
	 */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyAIController> AIController;
};

/**
 * This task is being used in the State Tree
 * it sends an event to the state tree to transition from state to another state
 */
USTRUCT(BlueprintType)
struct MI498_UEPROJECT_API FReleaseSwingSplineTask : public FStateTreeTaskCommonBase
{
public:
	GENERATED_BODY()
	using FInstanceDataType = FReleaseSwingSplineTaskInstanceData;

	/**
	 * Gets the type of instance data used by this task.
	 * @return The UStruct representing the instance data type.
	 */
	virtual const UStruct* GetInstanceDataType() const override;

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

};
