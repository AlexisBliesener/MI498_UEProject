// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "MI498_UEProject/Characters/Enemies/SwingingEnemy/SwingingEnemy.h"
#include "PlayerLocationEvaluator.generated.h"

class AEnemyAIController;
/**
 * This evaluator is the brain to the state tree that is going to get location about the player
 * and let other tasks know about it 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Player Location Evaluator",category = "SwingingEnemy"))
class MI498_UEPROJECT_API UPlayerLocationEvaluator : public UStateTreeEvaluatorBlueprintBase
{
public:
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
	
	
	
	/**
	 * Is player under enemy?
	 */
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= Output, meta = (AllowPrivateAccess = "true"))
	bool bPlayerUnderEnemy = false;	
	/**
	 * Is player in x and y range? it ignore z axis
	 */
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= Output,  meta = (AllowPrivateAccess = "true"))
	bool bPlayerInXYRange = false;
	/**
	 * if this true, it will trigger melee attack 
	 */
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= Output,  meta = (AllowPrivateAccess = "true"))
	bool bPlayerInMeleeRange = false;
	/**
	 * Cache the player for attack purpose 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> Player;
	/**
	 * Prepares the AI controller and enemy actor for executing the state tree.
	 *
	 * @param Context The execution context provided by the state tree.
	 */
	virtual void TreeStart(FStateTreeExecutionContext& Context) override;
	/**
	 * Stops the state tree logic and cleans up AI perception events.
	 *
	 * @param Context The execution context provided by the state tree.
	 */
	virtual void TreeStop(FStateTreeExecutionContext& Context) override;
	
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
};
