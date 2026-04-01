// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Prediction.h"
#include "AITypes.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AISenseConfig_Sight.h"

DEFINE_LOG_CATEGORY(EnemyAILog);
AEnemyAIController::AEnemyAIController()
{
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	bAttachToPawn = true;
	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeEnemyComponent>(TEXT("StateTreeEnemyComponent"));
	BrainComponent = StateTreeAIComponent;
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 1000.0f;
    SightConfig->LoseSightRadius = 1500.0f;
    SightConfig->PeripheralVisionAngleDegrees = 90.f;
    SightConfig->SetMaxAge(5.f); 
    // SightConfig->PointOfViewBackwardOffset = 150.0f; )
    // SightConfig->NearClippingRadius = 90.0f;
    SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true; 
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true; 
    SightConfig->DetectionByAffiliation.bDetectNeutrals = false; 

    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());
	
    // damage perception
    DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
    PerceptionComponent->ConfigureSense(*DamageConfig);

    // prediction 
    PredictionConfig = CreateDefaultSubobject<UAISenseConfig_Prediction>(TEXT("PredictionConfig"));
    PredictionConfig->SetMaxAge(1.0f); 
    PredictionConfig->SetStartsEnabled(true); 
    PerceptionComponent->ConfigureSense(*PredictionConfig);
    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
    PerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionForgotten);
}

FPathFollowingRequestResult AEnemyAIController::MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath)
{
	AEnemyBase* enemy = Cast<AEnemyBase>(GetPawn());
	if (enemy && enemy->RealShip && enemy->HiddenShip)
	{
		FVector realTarget;
       
		// check if the "move to" is actor or a location 
		if (MoveRequest.IsMoveToActorRequest() && MoveRequest.GetGoalActor())
		{
			realTarget = MoveRequest.GetGoalActor()->GetActorLocation();
			// we don't want to use it anymore since we are doing a ProjectPointToNavigation, the ai will get the nearest point to that location in moveto funciton  
			//realTarget.Z = enemy->GetActorLocation().Z; // ignore the height in enemy movement since most of the time the player will be in the air 
		}
		else 
		{
			realTarget = MoveRequest.GetGoalLocation();
		}

		FVector hiddenTarget;

		float distToFakeShip = FVector::DistSquared(realTarget, enemy->HiddenShip->GetActorLocation());
		float distToRealShip = FVector::DistSquared(realTarget, enemy->RealShip->GetActorLocation());

		// check if the point is on the fake ship or the real one... 
		if (distToFakeShip < distToRealShip)
		{
			hiddenTarget = realTarget;
		}
		else
		{
			FVector localTarget = enemy->RealShip->GetActorTransform().InverseTransformPosition(realTarget);
			hiddenTarget = enemy->HiddenShip->GetActorTransform().TransformPosition(localTarget);
		}
		
		// it's taking a reference of the hiddentarget and changing it to a valid point if the hiddentarget was not on a valid navmesh point!
		CheckAndGetAValidPointOnNavMesh(hiddenTarget, realTarget);
		
		
		// This fixes when the MoveTo task has a target as an ACTOR not a destination  
		// because we're using the fake ship for the navmesh, so we need to ignore the given actor and translate it to the fake ship
		// and unreal doesn't allow changing FAIMoveRequest to use a destination instead of an actor after it's created
		// so we had to create a new move request and pass the old params to the new one and this is how lovely unreal is...
		FAIMoveRequest newRequest(hiddenTarget);
		newRequest.SetAcceptanceRadius(MoveRequest.GetAcceptanceRadius());
		newRequest.SetUsePathfinding(MoveRequest.IsUsingPathfinding());
		newRequest.SetAllowPartialPath(MoveRequest.IsUsingPartialPaths());
		newRequest.SetProjectGoalLocation(MoveRequest.IsProjectingGoal());
		newRequest.SetNavigationFilter(MoveRequest.GetNavigationFilter());
		newRequest.SetCanStrafe(MoveRequest.CanStrafe());
		newRequest.SetReachTestIncludesAgentRadius(MoveRequest.IsReachTestIncludingAgentRadius());
		newRequest.SetReachTestIncludesGoalRadius(MoveRequest.IsReachTestIncludingGoalRadius());
		newRequest.SetRequireNavigableEndLocation(MoveRequest.IsNavigableEndLocationRequired());
       
		return Super::MoveTo(newRequest, OutPath);
	}

	return Super::MoveTo(MoveRequest, OutPath);
}
void AEnemyAIController::CheckAndGetAValidPointOnNavMesh(FVector& TargetPoint, FVector& RealTarget)
{
	AEnemyBase* enemy = Cast<AEnemyBase>(GetPawn());
	if (!enemy || !enemy->RealShip || !enemy->HiddenShip) return;

	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (navSys)
	{
		FNavLocation projectedTarget;
        
		FVector searchExtent(100.f, 100.f, 250.f); 
		// change the point only when it's not in the navmesh!! (We don't want to change the original point that was on the navmesh!!)
		if (navSys->ProjectPointToNavigation(TargetPoint, projectedTarget, searchExtent))
		{
			TargetPoint = projectedTarget.Location;
		}
		else
		{
			FVector localEnemyPos = enemy->RealShip->GetActorTransform().InverseTransformPosition(enemy->GetActorLocation());
			FVector enemyHiddenLocation = enemy->HiddenShip->GetActorTransform().TransformPosition(localEnemyPos);

			FVector hitLocation;
			if (navSys->NavigationRaycast(this, enemyHiddenLocation, TargetPoint, hitLocation))
			{
				TargetPoint = hitLocation;
			}
		}
	}
}
UStateTreeEnemyComponent* AEnemyAIController::GetStateTreeAIComponent() const
{
	return StateTreeAIComponent;
}

FGenericTeamId AEnemyAIController::GetGenericTeamId() const
{
	return FGenericTeamId(1);
}

void AEnemyAIController::ReportDamageEvent(AActor* DamagedActor, AActor* InstigatorActor, float DamageAmount)
{
	UWorld* World = GetWorld();
	if (!World || !DamagedActor)
	{
		UE_LOG(EnemyAILog, Warning, TEXT("no world or no damaged actor."));
		return;
	}

	const FVector EventLocation = DamagedActor->GetActorLocation();
	const FVector InstigatorLocation = InstigatorActor
		? InstigatorActor->GetActorLocation()
		: EventLocation;

	UAISense_Damage::ReportDamageEvent(
		World,
		DamagedActor,
		InstigatorActor,
		DamageAmount,
		EventLocation,
		InstigatorLocation
	);
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (AEnemyBase* enemyBase = Cast<AEnemyBase>(InPawn))
	{
		if (PerceptionComponent)
		{
			FAISenseID sightID = UAISense::GetSenseID(UAISense_Sight::StaticClass());
			if (UAISenseConfig_Sight* ActiveSightConfig = Cast<UAISenseConfig_Sight>(PerceptionComponent->GetSenseConfig(sightID)))
			{
				ActiveSightConfig->SightRadius = enemyBase->AttackStartDistance;
              
				ActiveSightConfig->LoseSightRadius = enemyBase->LoseSightRadius;
				ActiveSightConfig->AutoSuccessRangeFromLastSeenLocation = enemyBase->AutoSuccessRange;
				PerceptionComponent->ConfigureSense(*ActiveSightConfig);
			}
			else
			{
				UE_LOG(EnemyAILog, Warning, TEXT("active sight config not found"));
			}
		}
		else
		{
			UE_LOG(EnemyAILog, Warning, TEXT("PerceptionComponent is null!"));
		}
		if (!StateTreeAIComponent)
		{
			UE_LOG(EnemyAILog, Warning, TEXT("no StateTreeAIComponent for enemy ai controller."));
			return;
		}
		StateTreeAIComponent->StartStateTree(enemyBase->GetStateTree());
	}
	else
	{
		UE_LOG(EnemyAILog, Warning, TEXT("The enemy ai controller is not attached to an enemy base class!"));
	}
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor)
    {
        return;
    }
    static const FAISenseID sightID = UAISense::GetSenseID(UAISense_Sight::StaticClass());
    static const FAISenseID damageID = UAISense::GetSenseID(UAISense_Damage::StaticClass());
    AActor* SensedActor = Actor;
    if (!SensedActor)
    {
        UE_LOG(EnemyAILog, Error, TEXT("Actor %s is not a valid actor."), *GetNameSafe(Actor));
        return;
    }
	if (Stimulus.Type == damageID)
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			OnDamageStimulusDetected.Broadcast(SensedActor, Stimulus);
		}
	}
	else if (Stimulus.Type == sightID)
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			OnSightStimulusDetected.Broadcast(SensedActor, Stimulus);
		}
		else
		{
			OnSightStimulusForgotten.Broadcast(SensedActor);
		}
	}else{
		UE_LOG(EnemyAILog,Warning,TEXT("Unknown stimulus type for actor %s"),*GetNameSafe(Actor));
	}
}

void AEnemyAIController::OnTargetPerceptionForgotten(AActor* Actor)
{
	OnSightStimulusForgotten.Broadcast(Actor); 
}

void AEnemyAIController::SetFocalPoint(FVector NewFocus, EAIFocusPriority::Type InPriority)
{
	AEnemyBase* enemy = Cast<AEnemyBase>(GetPawn());
	if (enemy && enemy->RealShip && enemy->HiddenShip)
	{
		float distToFakeShip = FVector::DistSquared(NewFocus, enemy->HiddenShip->GetActorLocation());
		float distToRealShip = FVector::DistSquared(NewFocus, enemy->RealShip->GetActorLocation());

		if (distToFakeShip < distToRealShip)
		{
			// translate to the real ship
			FVector localPos = enemy->HiddenShip->GetActorTransform().InverseTransformPosition(NewFocus);
			FVector realFocus = enemy->RealShip->GetActorTransform().TransformPosition(localPos);
            
			Super::SetFocalPoint(realFocus, InPriority);
			return;
		}
	}

	// If it's already on the real ship 
	Super::SetFocalPoint(NewFocus, InPriority);
}