#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "SendEventTask.generated.h"


/**
 * StateTree instance data for the Get Random Location task.
 */
USTRUCT(BlueprintType,meta = (DisplayName = "Send Event Task Data"))
struct FSendEventTaskInstanceData
{
	GENERATED_BODY()

	/**
	 * Pointer to an enemy actor instance.
	 */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyBase> Actor;

	/**
	 * Pointer to the AI controller for an enemy.
	 */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyAIController> AIController;
	/**
	 * Tag used to identify and send specific events in the task.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	FGameplayTag EventTag;
};

/**
 * This task is being used in the State Tree
 * it sends an event to the state tree to transition from state to another state
 */
USTRUCT(BlueprintType)
struct MI498_UEPROJECT_API FSendEventTask : public FStateTreeTaskCommonBase
{
public:
	GENERATED_BODY()
	using FInstanceDataType = FSendEventTaskInstanceData;

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
