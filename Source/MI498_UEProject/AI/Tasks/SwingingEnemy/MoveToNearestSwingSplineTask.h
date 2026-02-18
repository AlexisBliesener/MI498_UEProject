// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MoveToNearestSwingSplineTask.generated.h"


class ASwingingEnemy;
/**
 * StateTree instance data for the MoveToNearestSwingSplineTask task.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "MoveToNearestSwingSplineTask"))
struct FMoveToNearestSwingSplineTaskInstanceData
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
 * it 
 */
USTRUCT(BlueprintType)
struct MI498_UEPROJECT_API FMoveToNearestSwingSplineTask : public FStateTreeTaskCommonBase
{
public:
	GENERATED_BODY()
	using FInstanceDataType = FMoveToNearestSwingSplineTaskInstanceData;

	/**
	 * Gets the type of instance data used by this task.
	 * @return The UStruct representing the instance data type.
	 */
	virtual const UStruct* GetInstanceDataType() const override;
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
	 * Finds the nearest valid navigation location within a specified radius from a desired point.
	 *
	 * @param World The world ref
	 * @param DesiredLocation The initial target location to find a valid navigation point
	 * @param MaxReservePointRadius The maximum radius around the desired location for a search 
	 * @return Fvector if closest valid navigation location found, otherwise the original desired location
	 */
	static FVector GetNearestNavLocation(UWorld* World, const FVector& DesiredLocation, const float& MaxReservePointRadius);


};
