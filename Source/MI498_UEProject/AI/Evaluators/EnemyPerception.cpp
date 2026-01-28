// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPerception.h"
#include "StateTreeExecutionContext.h"
#include "MI498_UEProject/AI/EnemyAIController.h"

void UEnemyPerception::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);
	UE_LOG(LogTemp, Error, TEXT("TREE STARTED mmmmm!"));
	// set a context sdo we can access it from the task 
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

	AIController->CurrentStateTreeState = StateTreeEnemyEvents::Unknown; // Update the ai controller state tree state
	

	// get the Pawn from the ai controller
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
	UE_LOG(LogTemp, Error, TEXT("AAAAAAAAA!!"));
	if (APlayerCharacter* player = Cast<APlayerCharacter>(TargetActor))
	{
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
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,10.00f,FColor::Yellow,FString::Printf(TEXT("no sight du!!")));
	}
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
	UE_LOG(LogTemp, Log, TEXT("Hearing forgotten for actor: %s"), *GetNameSafe(TargetActor));
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
	UE_LOG(LogTemp, Error, TEXT("AAAAAA!! EVENT SENTNTTTT"));
	FGameplayTag EventTag;
	switch (Event)
	{
	case StateTreeEnemyEvents::Idle:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Event.Idle"));
		UE_LOG(LogTemp, Error, TEXT("TO IDLE !!"));

		LastEvent = StateTreeEnemyEvents::Idle;
		break;
	case StateTreeEnemyEvents::Attack:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Event.Attack"));
		UE_LOG(LogTemp, Error, TEXT("TO ATTACK!!"));

		LastEvent = StateTreeEnemyEvents::Attack;
		break;
	case StateTreeEnemyEvents::Patrol:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Event.Patrol"));
		UE_LOG(LogTemp, Error, TEXT("TO PATROL!!"));

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


