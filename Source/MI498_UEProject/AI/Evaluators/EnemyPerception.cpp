// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPerception.h"

#include "NavigationSystem.h"
#include "StateTreeExecutionContext.h"
#include "MI498_UEProject/AI/EnemyAIController.h"

void UEnemyPerception::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);
	// Get the AI controller from the context (from a state tree)
	AEnemyAIController* aiController = Cast<AEnemyAIController>(Context.GetOwner());
	if (!aiController)
	{
		UE_LOG(EnemyAILog, Error, TEXT(" StateTree owner is not an EnemyAIController!"));
		return;
	}
	AIController = aiController;

	AIController->OnSightStimulusDetected.AddDynamic(this, &UEnemyPerception::HandleSightStimulus);
	AIController->OnSightStimulusForgotten.AddDynamic(this, &UEnemyPerception::HandleSightStimulusForgotten);
	AIController->OnDamageStimulusDetected.AddDynamic(this, &UEnemyPerception::HandleDamageStimulus);

	AIController->CurrentStateTreeState = StateTreeEnemyEvents::Unknown; 

	APawn* OwnerPawn = AIController->GetPawn();
	if (!OwnerPawn)
	{
		UE_LOG(EnemyAILog, Error, TEXT("AIController has no Pawn!"));
		return;
	}
	
	Actor = Cast<AEnemyBase>(OwnerPawn);
	if (!IsValid(Actor))
	{
		UE_LOG(EnemyAILog, Warning, TEXT("Pawn is not an AEnemyBase! Class is: %s"), *OwnerPawn->GetClass()->GetName());
		return;
	}
	
	StateTreeAIComponent = AIController->GetStateTreeAIComponent();
	if (!StateTreeAIComponent)
	{
		UE_LOG(EnemyAILog, Error, TEXT("No UStateTreeAIComponent found on Ai Controller!"));
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
		TargetPlayer = player;
		if (IsValid(AIController))
		{
			AIController->AcquiredTarget = player; // Clear the AIController target
		}
		// We only want to send the event to the state tree once to avoid duplicated events  
		if (AIController->CurrentStateTreeState != StateTreeEnemyEvents::Attack)
		{
			OnSightStimulus(TargetActor, Stimulus);

			SendEventToStateTree(StateTreeEnemyEvents::Attack); // Send event to StateTree
			AIController->CurrentStateTreeState = StateTreeEnemyEvents::Attack; 
		}
	}
}

void UEnemyPerception::HandleSightStimulusForgotten(AActor* TargetActor)
{
	if (TargetActor && IsValid(Actor) && Actor->RealShip && Actor->HiddenShip)
	{
		FVector realPos = TargetActor->GetActorLocation();
		FVector playerSpeed = TargetActor->GetVelocity();

		FVector localPos = Actor->RealShip->GetActorTransform().InverseTransformPosition(realPos);
		FVector localSpeed = Actor->RealShip->GetActorTransform().InverseTransformVector(playerSpeed);

		localSpeed.Z = 0.f;

		FVector predictedLocalPos = localPos + (localSpeed * 1.3f); // 1.3 is predicted time 

		FVector fakeLoc = Actor->HiddenShip->GetActorTransform().TransformPosition(predictedLocalPos);

		UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Actor->GetWorld());
		if (navSys)
		{
			FNavLocation groundLocation;
			FVector lookBox = FVector(500.f, 500.f, 10000.f);

			if (navSys->ProjectPointToNavigation(fakeLoc, groundLocation, lookBox))
			{
				fakeLoc = groundLocation.Location;
			}
			else
			{
				// if the location is outside the map after the predication then fallback to the ground loc 
				FVector currentFakeLoc = Actor->HiddenShip->GetActorTransform().TransformPosition(localPos);
				if (navSys->ProjectPointToNavigation(currentFakeLoc, groundLocation, lookBox))
				{
					fakeLoc = groundLocation.Location;
				}
				else
				{
					fakeLoc = currentFakeLoc; 
				}
			}
		}

		// translate the location to the real ship
		FVector localGround = Actor->HiddenShip->GetActorTransform().InverseTransformPosition(fakeLoc);
		FVector realGround = Actor->RealShip->GetActorTransform().TransformPosition(localGround);

		LastSeenLocation = realGround;
	}

	TargetPlayer = nullptr;
	
	SendEventToStateTree(StateTreeEnemyEvents::Search);

	if (IsValid(AIController))
	{
		AIController->CurrentStateTreeState = StateTreeEnemyEvents::Search;
		AIController->AcquiredTarget = nullptr;
	}

	OnSightStimulusForgotten(TargetActor);
	
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
	if (LastEvent == Event)
	{
		// duplicated event...
		return;
	}
	FGameplayTag EventTag;
	switch (Event)
	{
	case StateTreeEnemyEvents::Idle:
		return;
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Event.Idle"));

		LastEvent = StateTreeEnemyEvents::Idle;
		break;
	case StateTreeEnemyEvents::Attack:
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Event.Attack"));

		LastEvent = StateTreeEnemyEvents::Attack;
		break;
	case StateTreeEnemyEvents::Patrol:
		return;
		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Event.Patrol"));

		LastEvent = StateTreeEnemyEvents::Patrol;
		break;
	case StateTreeEnemyEvents::Search:

		EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Event.Search"));
		LastEvent = StateTreeEnemyEvents::Search;
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


