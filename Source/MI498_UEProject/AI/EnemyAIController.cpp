// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Prediction.h"
#include "Perception/AISenseConfig_Sight.h"

DEFINE_LOG_CATEGORY(EnemyLog);
AEnemyAIController::AEnemyAIController()
{
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	bAttachToPawn = true;
	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeEnemyComponent>(TEXT("BEEE"));
	BrainComponent = StateTreeAIComponent;
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 1000.0f;
    SightConfig->LoseSightRadius = 1500.0f;
    SightConfig->PeripheralVisionAngleDegrees = 35.0f;
    // SightConfig->SetMaxAge(2.f); 
    // SightConfig->PointOfViewBackwardOffset = 150.0f; )
    // SightConfig->NearClippingRadius = 90.0f;
    SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true; 
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true; 
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true; 

    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());

    // hearing
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 1200.f;
    // HearingConfig->SetMaxAge(3.f);
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    PerceptionComponent->ConfigureSense(*HearingConfig);

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

UStateTreeEnemyComponent* AEnemyAIController::GetStateTreeAIComponent() const
{
	return StateTreeAIComponent;
}

void AEnemyAIController::ReportDamageEvent(AActor* DamagedActor, AActor* InstigatorActor, float DamageAmount)
{
	UWorld* World = GetWorld();
	if (!World || !DamagedActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("no world or no damaged actor."));
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
	// StateTreeAIComponent->RestartLogic();
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (AEnemyBase* EnemyBase = Cast<AEnemyBase>(InPawn))
	{
		if (!StateTreeAIComponent)
		{
			GEngine->AddOnScreenDebugMessage(
	-1,
	5.0f,
	FColor::Red,
	TEXT("ERROR: StateTreeAiCompononet is not assigned MOhammed !!")
);
			return;
		}
		StateTreeAIComponent->StartStateTree(EnemyBase->GetStateTree());

		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Green,
			TEXT("SUCCESS: EnemyBase possessed")
		);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Red,
			TEXT("ERROR: Pawn is not EnemyBase")
		);
	}
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,  
			FColor::Yellow, 
			FString::Printf(TEXT("TargetPerceptionUpdated ")));
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("SOMETHING NOT FOUND TargetPerceptionUpdated!!"));
	}
    if (!Actor)
    {
        return;
    }
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 
			5.0f,  
			FColor::Yellow, 
			FString::Printf(TEXT("There is an acotr!! "))); 
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("SOMETHING NOT FOUND TargetPerceptionUpdated!!"));
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 
			5.0f,    
			FColor::Yellow, 
			FString::Printf(TEXT("IT CHECK FOR HOSTILE! MOHAMMMMEDD! ")));
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("SOMETHING NOT FOUND TargetPerceptionUpdated!!"));
	}
	
    static const FAISenseID SightID = UAISense::GetSenseID(UAISense_Sight::StaticClass());
    static const FAISenseID HearingID = UAISense::GetSenseID(UAISense_Hearing::StaticClass());
    static const FAISenseID DamageID = UAISense::GetSenseID(UAISense_Damage::StaticClass());

	UE_LOG(LogTemp, Error, TEXT("REACHED HEREEREERE!!"))
        FString SenseName;
    if (Stimulus.Type == SightID)
        SenseName = TEXT("Sight");
    else if (Stimulus.Type == HearingID)
        SenseName = TEXT("Hearing");
    else if (Stimulus.Type == DamageID)
        SenseName = TEXT("Damage");
    else {
        SenseName = TEXT("Unknown");
    }
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,        
			5.0f,    
			FColor::Yellow,
			SenseName);
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("SOMETHING NOT FOUND TargetPerceptionUpdated!!"));
	}
	
    AActor* SensedActor = Actor;
    if (!SensedActor)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Actor %s is not a valid actor."), *GetNameSafe(Actor));
        return;
    }
	// TODO: sw	
    if (Stimulus.Type == DamageID)
    {
    	if (GEngine)
    	{
    		GEngine->AddOnScreenDebugMessage(
				-1,    
				10.00f,     
				FColor::Yellow, 
				FString::Printf(TEXT("HERE AT DAMAGE ID")));
    	}else
    	{
    		UE_LOG(LogTemp, Error, TEXT("SOMETHING NOT FOUND TargetPerceptionUpdated!!"));
    	}
        if (Stimulus.WasSuccessfullySensed())
        {
            OnDamageStimulusDetected.Broadcast(SensedActor, Stimulus);
        }
    }
    else if (Stimulus.Type == SightID)
    {
    	if (GEngine)
    	{
    		GEngine->AddOnScreenDebugMessage(
				-1,       
				5.0f,      
				FColor::Yellow,
				FString::Printf(TEXT("here at sight id  ")));
    	}else
    	{
    		UE_LOG(LogTemp, Error, TEXT("SOMETHING NOT FOUND TargetPerceptionUpdated!!"));
    	}
        if (Stimulus.WasSuccessfullySensed())
        {
            OnSightStimulusDetected.Broadcast(SensedActor, Stimulus);
        }
        else
        {
            OnSightStimulusForgotten.Broadcast(SensedActor);
        }
    }
    else if (Stimulus.Type == HearingID)
    {
    	if (GEngine)
    	{
    		GEngine->AddOnScreenDebugMessage(
				-1,      
				5.0f,   
				FColor::Yellow,
				FString::Printf(TEXT("here at hearing id  "))); 
    	}else
    	{
    		UE_LOG(LogTemp, Error, TEXT("SOMETHING NOT FOUND TargetPerceptionUpdated!!"));
    	}
        if (Stimulus.WasSuccessfullySensed())
        {
            OnHearingStimulusDetected.Broadcast(SensedActor, Stimulus);
        }
        else
        {
            OnHearingStimulusForgotten.Broadcast(SensedActor);
        }
    }else
    {
    	if (GEngine)
    	{
    		GEngine->AddOnScreenDebugMessage(
				-1,     
				5.0f,     
				FColor::Yellow, 
				FString::Printf(TEXT("SImulation not found mohammed "))); 
    	}else
    	{
    		UE_LOG(LogTemp, Error, TEXT("SOMETHING NOT FOUND TargetPerceptionUpdated!!"));
    	}
    }
}

void AEnemyAIController::OnTargetPerceptionForgotten(AActor* Actor)
{
	OnSightStimulusForgotten.Broadcast(Actor); 
	OnHearingStimulusForgotten.Broadcast(Actor); 
}
