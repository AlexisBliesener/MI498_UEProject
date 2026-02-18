// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "ProbabilityAttackTask.generated.h"

/**
 * StateTree instance data for the ProbabilityAttackTask task.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "ProbabilityAttackTask"))
struct FProbabilityAttackTaskInstanceData
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
	 * Is this a secondary attack?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (Context, AllowDerivedTypes="true"))
	bool bIsSecondaryAttack = false;

	/**
	 * Pointer to the target actor used in the task.
	 */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<AActor> Target = nullptr;
	
	/**  
	 * Probability of the attack (0.0 to 1.0)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (ClampMin="0.0", ClampMax="1.0"))
	float ChanceToAttack = 0.1f;
};

/**
 * This task is being used in the State Tree
 * it 
 */
USTRUCT(BlueprintType)
struct MI498_UEPROJECT_API FProbabilityAttackTask : public FStateTreeTaskCommonBase
{
public:
	GENERATED_BODY()
	using FInstanceDataType = FProbabilityAttackTaskInstanceData;

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
