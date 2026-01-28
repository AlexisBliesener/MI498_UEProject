// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/StateTreeAIComponent.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
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
DECLARE_LOG_CATEGORY_EXTERN(EnemyLog, Log, All);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent, DisplayName = "Enemy AI Controller"))
class MI498_UEPROJECT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();
	FTimerHandle FuzeTimerHandle;
	UStateTreeEnemyComponent* GetStateTreeAIComponent() const;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Output, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> AcquiredTarget = nullptr;
	UPROPERTY(BlueprintAssignable, Category = "Player|AI|Events", meta = (DisplayName = "On Damage Stimulus Detected"))
	FPGASStimulusEventSignature OnDamageStimulusDetected;
	UPROPERTY(BlueprintAssignable, Category = "Player|AI|Events", meta = (DisplayName = "On Sight Stimulus Detected"))
	FPGASStimulusEventSignature OnSightStimulusDetected;
	UPROPERTY(BlueprintAssignable, Category = "Player|AI|Events", meta = (DisplayName = "On Sight Stimulus Forgotten"))
	FPGASStimulusForgottenSignature OnSightStimulusForgotten;
	UPROPERTY(BlueprintAssignable, Category = "Player|AI|Events", meta = (DisplayName = "On Hearing Stimulus Detected"))
	FPGASStimulusEventSignature OnHearingStimulusDetected;
	UPROPERTY(BlueprintAssignable, Category = "Player|AI|Events", meta = (DisplayName = "On Hearing Stimulus Forgotten"))
	FPGASStimulusForgottenSignature OnHearingStimulusForgotten;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Output, meta = (AllowPrivateAccess = "true"))
	StateTreeEnemyEvents CurrentStateTreeState;
	UFUNCTION(BlueprintCallable, Category = "AI|Perception")
	void ReportDamageEvent(AActor* DamagedActor, AActor* InstigatorActor, float DamageAmount);
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|References")
	TObjectPtr<UStateTreeEnemyComponent> StateTreeAIComponent;
	
	virtual void BeginPlay() override;
	
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void OnTargetPerceptionForgotten(AActor* Actor);
private:
	TObjectPtr<AEnemyBase> OwningCharacter = nullptr;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Damage> DamageConfig;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Prediction> PredictionConfig;
};
