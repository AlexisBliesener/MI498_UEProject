// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "PlayerEvaluator.generated.h"

class AEnemyAIController;
/**
 * This evaluator is the brain to the state tree that is going to get information about the player
 * and let other tasks know about it 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Player Evaluator",category = "AI"))
class MI498_UEPROJECT_API UPlayerEvaluator : public UStateTreeEvaluatorBlueprintBase
{
public:
	GENERATED_BODY()
	/**
	 * The attack range to trigger attack event
	 */
	UPROPERTY(VisibleAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	float AttackRange = 100.0f;
	
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
	 * Distance from Enemy to the Player Pawn
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output, meta = (AllowPrivateAccess = "true"))
	float DistanceToPlayer = 0.f;
	
	/**
	 * Is player in range for attack? 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output, meta = (AllowPrivateAccess = "true"))
	bool bIsPlayerInRange = false;
	
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
