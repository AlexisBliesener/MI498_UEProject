// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Characters/Enemies/SwingingEnemy/SwingingEnemy.h"
#include "WaitOnSwingStopTask.generated.h"

/**
 * StateTree instance data for the WaitOnSwingStopTask task.
 * This task was stolen from the built-in delay task in unreal engine....
 */
USTRUCT(BlueprintType, meta = (DisplayName = "WaitOnSwingStopTask"))
struct FWaitOnSwingStopTaskInstanceData
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
	
	
	/** Internal countdown in seconds. */
	float RemainingTime = 0.f;
	
	
	/** The handle of the scheduled tick request. */
	UE::StateTree::FScheduledTickHandle ScheduledTickHandle;
};

/**
 * This task is being used in the State Tree
 * it 
 */
USTRUCT(BlueprintType)
struct MI498_UEPROJECT_API FWaitOnSwingStopTask : public FStateTreeTaskCommonBase
{
public:
	GENERATED_BODY()
	using FInstanceDataType = FWaitOnSwingStopTaskInstanceData;

	/**
	 * Gets the type of instance data used by this task.
	 * @return The UStruct representing the instance data type.
	 */
	virtual const UStruct* GetInstanceDataType() const override;
	FWaitOnSwingStopTask();
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
	
	
	
#if WITH_EDITOR
	virtual FName GetIconName() const override
	{
		return FName("StateTreeEditorStyle|Node.Time");
	}
	virtual FColor GetIconColor() const override
	{
		return UE::StateTree::Colors::Grey;
	}
#endif
};
