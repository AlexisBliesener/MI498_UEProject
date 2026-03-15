// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "SetAssignedLocation.generated.h"

/**
 * StateTree instance data for the SetAssignedLocation task.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "SetAssignedLocation"))
struct FSetAssignedLocationInstanceData
{
	GENERATED_BODY()

	/**
	 * Points to the enemy character being controlled by the AI.
	 */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyBase> Actor;

	/**
	 * Points to the AI controller responsible for controlling the enemy actor.
	 */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyAIController> AIController;
	
	/**
	 * The location to assign it to the enemy 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FVector AssignedLocation = FVector::ZeroVector;
};

/**
 * This task is being used in the State Tree
 * it 
 */
USTRUCT(BlueprintType)
struct MI498_UEPROJECT_API FSetAssignedLocation : public FStateTreeTaskCommonBase
{
public:
	GENERATED_BODY()
	using FInstanceDataType = FSetAssignedLocationInstanceData;

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
