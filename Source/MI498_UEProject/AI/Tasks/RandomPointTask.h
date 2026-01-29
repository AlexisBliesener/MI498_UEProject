#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "RandomPointTask.generated.h"


/**
 * Holds data for the Get Random Location Task.
 */
USTRUCT(BlueprintType,meta = (DisplayName = "Get Random Location Data"))
struct FGetRandomLocationTaskInstanceData
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
	 * Defines the search radius for generating a random location.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	float SearchRadius = 1000.f;

	/**
	 * Stores a random location found during the task. This is used as an output
	 * which means it sends it to another task (MoveAi)  
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Output, meta = (Output))
	FVector RandomLocation = FVector::ZeroVector;
};

/**
 * This task is being used in the State Tree
 * it gets a random point around the enemy and return it 
 */
USTRUCT(BlueprintType)
struct MI498_UEPROJECT_API FRandomPointTask : public FStateTreeTaskCommonBase
{
public:
	GENERATED_BODY()
	using FInstanceDataType = FGetRandomLocationTaskInstanceData;

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
