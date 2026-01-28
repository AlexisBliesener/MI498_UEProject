#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "PreformAttackTask.generated.h"


/**
 * Holds data for the Perform Attack Task.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Get Random Location Data"))
struct FPreformAttackTaskInstanceData
{
	GENERATED_BODY()

	/**
	 * Pointer to an enemy actor used in the attack task.
	 */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyBase> Actor;

	/**
	 * Holds a reference to the Enemy AI Controller.
	 */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyAIController> AIController;

	/**
	 * Pointer to the target actor used in the task.
	 */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<AActor> Target = nullptr;
};

/**
 * Executes a state tree task for managing attack behavior.
 */
USTRUCT(BlueprintType)
struct MI498_UEPROJECT_API FPreformAttackTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FPreformAttackTaskInstanceData;

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
