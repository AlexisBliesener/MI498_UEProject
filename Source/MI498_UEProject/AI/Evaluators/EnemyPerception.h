// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "Components/StateTreeAIComponent.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "MI498_UEProject/AI/Enums/StateTreeEnemyEvents.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"
#include "EnemyPerception.generated.h"

class AEnemyAIController;
/**
 * This evaluator is the brain to the state tree that is going to set the target (the player) 
 * and let other tasks know about it 
 */
UCLASS(Blueprintable, meta = (DisplayName = "AI Perception Evaluator",category = "AI"))
class MI498_UEPROJECT_API UEnemyPerception : public UStateTreeEvaluatorBlueprintBase
{
public:
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
	 * Stores the player character that is currently targeted by the AI.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Output, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<APlayerCharacter> TargetPlayer = nullptr;

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
protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
	/// Called when sight stimulus is detected
	void OnSightStimulus(AActor* TargetActor, const FAIStimulus& Stimulus);

	/// Called when sight stimulus is forgotten
	UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
	void OnSightStimulusForgotten(AActor* TargetActor);

	/// Called when hearing stimulus is detected 
	UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
	void OnHearingStimulus(AActor* TargetActor, const FAIStimulus& Stimulus);

	/// Called when hearing stimulus is forgotten 
	UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
	void OnHearingStimulusForgotten(AActor* TargetActor);

	/// Called when damage stimulus is detected
	UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
	void OnDamageStimulus(AActor* TargetActor, const FAIStimulus& Stimulus);
	
private:
	/**
	 * A component used by the state tree to manage AI behavior for the enemy.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponent;

	/**
	 * Keeps track of the last behavior event of the enemy AI
	 */
	StateTreeEnemyEvents LastEvent = StateTreeEnemyEvents::Idle;
	/**
	 * Called when a sight stimulus is detected to manage player targeting and trigger state tree events.
	 *
	 * @param TargetActor The actor that triggered the sight stimulus.
	 * @param Stimulus The details about the sight stimulus.
	 */
	UFUNCTION()
	void HandleSightStimulus(AActor* TargetActor, const FAIStimulus& Stimulus);

	/**
	 * Called when a sight stimulus is no longer detected to reset the AI state and update the state tree.
	 *
	 * @param TargetActor The actor whose sight stimulus was forgotten.
	 */
	UFUNCTION()
	void HandleSightStimulusForgotten(AActor* TargetActor);

	/**
	 * Handles hearing stimulus events to update the target and trigger state transitions.
	 *
	 * @param TargetActor The actor that caused the hearing stimulus.
	 * @param Stimulus Information about the hearing stimulus.
	 */
	UFUNCTION()
	void HandleHearingStimulus(AActor* TargetActor, const FAIStimulus& Stimulus);

	/**
	 * Handles the event when a hearing stimulus is no longer detected by the AI.
	 *
	 * @param TargetActor The actor whose hearing stimulus was forgotten.
	 */
	UFUNCTION()
	void HandleHearingStimulusForgotten(AActor* TargetActor);

	/**
	 * Handles the detection of a damage stimulus and triggers state changes if required.
	 *
	 * @param TargetActor The actor that caused the damage stimulus.
	 * @param Stimulus Information about the detected damage stimulus.
	 */
	UFUNCTION()
	void HandleDamageStimulus(AActor* TargetActor, const FAIStimulus& Stimulus);

	/**
	 * Sends an event to the AI state tree based on the given enemy action.
	 *
	 * @param Event The specific event to send to the state tree
	 */
	UFUNCTION()
	void SendEventToStateTree(const StateTreeEnemyEvents Event);
};
