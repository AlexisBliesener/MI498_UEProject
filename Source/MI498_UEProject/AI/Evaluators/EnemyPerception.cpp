// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPerception.h"
#include "StateTreeExecutionContext.h"
#include "MI498_UEProject/AI/EnemyAIController.h"

void UEnemyPerception::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);
	// Get the AI controller from the context (from a state tree)
	AEnemyAIController* aiController = Cast<AEnemyAIController>(Context.GetOwner());
	if (!aiController)
	{
		UE_LOG(LogTemp, Error, TEXT(" StateTree owner is not an EnemyAIController!"));
		return;
	}
	AIController = aiController;

	AIController->OnSightStimulusDetected.AddDynamic(this, &UEnemyPerception::HandleSightStimulus);
	AIController->OnSightStimulusForgotten.AddDynamic(this, &UEnemyPerception::HandleSightStimulusForgotten);
	AIController->OnHearingStimulusDetected.AddDynamic(this, &UEnemyPerception::HandleHearingStimulus);
	AIController->OnHearingStimulusForgotten.AddDynamic(this, &UEnemyPerception::HandleHearingStimulusForgotten);
	AIController->OnDamageStimulusDetected.AddDynamic(this, &UEnemyPerception::HandleDamageStimulus);

	AIController->CurrentStateTreeState = StateTreeEnemyEvents::Unknown; 

	APawn* OwnerPawn = AIController->GetPawn();
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("AIController has no Pawn!"));
		return;
	}
	
	Actor = Cast<AEnemyBase>(OwnerPawn);
	if (!IsValid(Actor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn is not an AEnemyBase! Class is: %s"), *OwnerPawn->GetClass()->GetName());
		return;
	}
	
	StateTreeAIComponent = AIController->FindComponentByClass<UStateTreeAIComponent>();
	if (!StateTreeAIComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("No UStateTreeAIComponent found on Ai Controller!"));
		return;
	}
}

void UEnemyPerception::TreeStop(FStateTreeExecutionContext& Context)
{
	Super::TreeStop(Context);
	
	if (IsValid(AIController))
	{
		AIController->OnSightStimulusDetected.RemoveDynamic(this, &UEnemyPerception::HandleSightStimulus);
		AIController->OnSightStimulusForgotten.RemoveDynamic(this, &UEnemyPerception::HandleSightStimulusForgotten);
		AIController->OnHearingStimulusDetected.RemoveDynamic(this, &UEnemyPerception::HandleHearingStimulus);
		AIController->OnHearingStimulusForgotten.RemoveDynamic(this, &UEnemyPerception::HandleHearingStimulusForgotten);
		AIController->OnDamageStimulusDetected.RemoveDynamic(this, &UEnemyPerception::HandleDamageStimulus);
		SendEventToStateTree(StateTreeEnemyEvents::Idle);
		AIController->CurrentStateTreeState = StateTreeEnemyEvents::Idle;
	}

	StateTreeAIComponent = nullptr; // clear reference
	TargetPlayer = nullptr; // clear the target player
	
}

void UEnemyPerception::HandleSightStimulus(AActor* TargetActor, const FAIStimulus& Stimulus)
{
	if (APlayerCharacter* player = Cast<APlayerCharacter>(TargetActor))
	{
		// We only want to send the event to the state tree once to avoid duplicated events  
		if (AIController->CurrentStateTreeState != StateTreeEnemyEvents::Attack)
		{
			TargetPlayer = player;	
			if (IsValid(AIController))
			{
				AIController->AcquiredTarget = TargetActor; // Clear the AIController target
			}
			OnSightStimulus(TargetActor, Stimulus);

			SendEventToStateTree(StateTreeEnemyEvents::Attack); // Send event to StateTree
			AIController->CurrentStateTreeState = StateTreeEnemyEvents::Attack; 
		}
	}
}

void UEnemyPerception::HandleSightStimulusForgotten(AActor* TargetActor)
{
	TargetPlayer = nullptr;
	if (IsValid(AIController))
	{
		AIController->AcquiredTarget = nullptr; 
	}
	SendEventToStateTree(StateTreeEnemyEvents::Patrol);
	
	AIController->CurrentStateTreeState = StateTreeEnemyEvents::Patrol;
	OnSightStimulusForgotten(TargetActor);
}

void UEnemyPerception::HandleHearingStimulus(AActor* TargetActor, const FAIStimulus& Stimulus)
{
	if (APlayerCharacter* player = Cast<APlayerCharacter>(TargetActor))
	{
		if (AIController->CurrentStateTreeState != StateTreeEnemyEvents::Attack)
		{
			TargetPlayer = player;
			if (IsValid(AIController))
			{
				AIController->AcquiredTarget = TargetActor; 
			}
			OnHearingStimulus(TargetActor, Stimulus);
			SendEventToStateTree(StateTreeEnemyEvents::Attack);
			AIController->CurrentStateTreeState = StateTreeEnemyEvents::Attack;
		}
	}
}

void UEnemyPerception::HandleHearingStimulusForgotten(AActor* TargetActor)
{
	TargetPlayer = nullptr;
	if (IsValid(AIController))
	{
		AIController->AcquiredTarget = nullptr; 
	}
	AIController->CurrentStateTreeState = StateTreeEnemyEvents::Patrol; 
	SendEventToStateTree(StateTreeEnemyEvents::Patrol);
	OnHearingStimulusForgotten(TargetActor);
}

void UEnemyPerception::HandleDamageStimulus(AActor* TargetActor, const FAIStimulus& Stimulus)
{
	if (APlayerCharacter* player = Cast<APlayerCharacter>(TargetActor))
	{
		if (AIController->CurrentStateTreeState != StateTreeEnemyEvents::Attack)
		{
			TargetPlayer = player;
			if (IsValid(AIController))
			{
				AIController->AcquiredTarget = TargetActor; 
			}
			OnDamageStimulus(TargetActor, Stimulus);

			SendEventToStateTree(StateTreeEnemyEvents::Attack);
			AIController->CurrentStateTreeState = StateTreeEnemyEvents::Attack;
		}
	}
}

void UEnemyPerception::SendEventToStateTree(const StateTreeEnemyEvents Event)
{
	FGameplayTag EventTag;
	switch (Event)
	{
	case StateTreeEnemyEvents::Idle:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Event.Idle"));

		LastEvent = StateTreeEnemyEvents::Idle;
		break;
	case StateTreeEnemyEvents::Attack:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Event.Attack"));

		LastEvent = StateTreeEnemyEvents::Attack;
		break;
	case StateTreeEnemyEvents::Patrol:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Event.Patrol"));

		LastEvent = StateTreeEnemyEvents::Patrol;
		break;
	default:
		return; 
	}

	// send the event to the StateTree component if it exists
	if (IsValid(AIController) && StateTreeAIComponent)
	{
		FStateTreeEvent event(EventTag);
		StateTreeAIComponent->SendStateTreeEvent(event);
	}
}


