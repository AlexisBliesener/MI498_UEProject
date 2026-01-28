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
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "AI Perception Evaluator",category = "AI"))
class MI498_UEPROJECT_API UEnemyPerception : public UStateTreeEvaluatorBlueprintBase
{
public:
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyBase> Actor;

	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyAIController> AIController;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Output, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<APlayerCharacter> TargetPlayer = nullptr;
	
	virtual void TreeStart(FStateTreeExecutionContext& Context) override;
	virtual void TreeStop(FStateTreeExecutionContext& Context) override;
protected:
	/// These events are for blueprint... if we hated c++ and wanted to move to blueprint 
	// Called when sight stimulus is detected 
	UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
	void OnSightStimulus(AActor* TargetActor, const FAIStimulus& Stimulus);

	// Called when sight stimulus is forgotten
	UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
	void OnSightStimulusForgotten(AActor* TargetActor);

	// Called when hearing stimulus is detected 
	UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
	void OnHearingStimulus(AActor* TargetActor, const FAIStimulus& Stimulus);

	// Called when hearing stimulus is forgotten 
	UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
	void OnHearingStimulusForgotten(AActor* TargetActor);

	/// Called when damage stimulus is detected
	UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
	void OnDamageStimulus(AActor* TargetActor, const FAIStimulus& Stimulus);
	
private:
	UFUNCTION()
	void HandleSightStimulus(AActor* TargetActor, const FAIStimulus& Stimulus);

	UFUNCTION()
	void HandleSightStimulusForgotten(AActor* TargetActor);

	UFUNCTION()
	void HandleHearingStimulus(AActor* TargetActor, const FAIStimulus& Stimulus);

	UFUNCTION()
	void HandleHearingStimulusForgotten(AActor* TargetActor);

	UFUNCTION()
	void HandleDamageStimulus(AActor* TargetActor, const FAIStimulus& Stimulus);
	
	UFUNCTION()
	void SendEventToStateTree(const StateTreeEnemyEvents Event);

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponent;
	
	StateTreeEnemyEvents LastEvent = StateTreeEnemyEvents::Idle;
};
