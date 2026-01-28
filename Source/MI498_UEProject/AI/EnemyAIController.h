// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MI498_UEProject/AI/Components/StateTreeEnemyComponent.h"
#include "EnemyAIController.generated.h"


enum class StateTreeEnemyEvents : uint8;
class AEnemyBase;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAISenseConfig_Damage;
class UAISenseConfig_Prediction;
class UAIPerceptionComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPGASStimulusEventSignature, AActor*, Actor, const FAIStimulus&, Stimulus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPGASStimulusForgottenSignature, AActor*, Actor);
/// Log category for enemy messages
DECLARE_LOG_CATEGORY_EXTERN(EnemyAILog, Log, All);

/**
 * Enemy AI Controller class handles AI behaviors, including perception updates,
 *  and state tree management for the enemy
 */
UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent, DisplayName = "Enemy AI Controller"))
class MI498_UEPROJECT_API AEnemyAIController : public AAIController
{
public:
	GENERATED_BODY()
	// Holds the actor currently targeted by the AI.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Output, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> AcquiredTarget = nullptr;
	// Triggered when the AI detects a damage stimulus.
	UPROPERTY(BlueprintAssignable, Category = "Player|AI|Events", meta = (DisplayName = "On Damage Stimulus Detected"))
	FPGASStimulusEventSignature OnDamageStimulusDetected;
	// Triggered when the AI detects a sight stimulus.
	UPROPERTY(BlueprintAssignable, Category = "Player|AI|Events", meta = (DisplayName = "On Sight Stimulus Detected"))
	FPGASStimulusEventSignature OnSightStimulusDetected;
	// Triggered when the AI loses sight of the target
	UPROPERTY(BlueprintAssignable, Category = "Player|AI|Events", meta = (DisplayName = "On Sight Stimulus Forgotten"))
	FPGASStimulusForgottenSignature OnSightStimulusForgotten;
	// Triggered when the AI detects a hearing stimulus.
	UPROPERTY(BlueprintAssignable, Category = "Player|AI|Events", meta = (DisplayName = "On Hearing Stimulus Detected"))
	FPGASStimulusEventSignature OnHearingStimulusDetected;
	// Triggered when the AI loses hearing of the target
	UPROPERTY(BlueprintAssignable, Category = "Player|AI|Events", meta = (DisplayName = "On Hearing Stimulus Forgotten"))
	FPGASStimulusForgottenSignature OnHearingStimulusForgotten;
	// The current state of the AI's behavior
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Output, meta = (AllowPrivateAccess = "true"))
	StateTreeEnemyEvents CurrentStateTreeState;

	/**
	* initializes the enemy AI controller by setting up the components 
	*/
	AEnemyAIController();
	/**
	 * Reports a damage event to the AI perception system.
	 * @param DamagedActor The actor that received the damage.
	 * @param InstigatorActor The actor that caused the damage.
	 * @param DamageAmount The amount of damage dealt.
	 */
	UFUNCTION(BlueprintCallable, Category = "AI|Perception")
	void ReportDamageEvent(AActor* DamagedActor, AActor* InstigatorActor, float DamageAmount);
	/**
	 * Gets the StateTree AI component associated with this AI controller.
	 * @return A pointer to the UStateTreeEnemyComponent, or nullptr if not.
	 */
	UStateTreeEnemyComponent* GetStateTreeAIComponent() const;
	
protected:
	// A reference to the AI's StateTree component 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|References")
	TObjectPtr<UStateTreeEnemyComponent> StateTreeAIComponent;
	
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	/**
	 * Called when the AI perception system updates information about a perceived actor.
	 * @param Actor The actor that was perceived or updated.
	 * @param Stimulus The stimulus data describing how the actor was perceived.
	 */
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	/**
	 * Called when the AI forgets about a previously perceived actor.
	 * @param Actor The actor that was forgotten by the AI perception system.
	 */
	UFUNCTION()
	void OnTargetPerceptionForgotten(AActor* Actor);
private:
	// Configuration for the AI's sight sense. 
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	// Configuration for the AI's Hearing sense. 
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;
	// Configuration for the AI's Damage sense. 
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Damage> DamageConfig;
	// Configuration for the AI's Prediction sense. 
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Prediction> PredictionConfig;
};
