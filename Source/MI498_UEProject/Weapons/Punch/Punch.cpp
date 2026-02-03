// Fill out your copyright notice in the Description page of Project Settings.


#include "Punch.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"


void APunch::PrimaryAttack(AController* Controller, AActor* Target)
{
	Super::PrimaryAttack(Controller, Target);
	if (!Controller || !Controller->GetPawn()) return;
	
	// TODO: Change that to animation notify state when animation is ready!!
	AActor* ownerPawn = Controller->GetPawn();
    
	FVector startLocation = ownerPawn->GetActorLocation(); // this is the start location from the enemy
	FVector forwardLocation = ownerPawn->GetActorForwardVector(); // forward location from the enemy 
	FVector endLocation = startLocation + (forwardLocation * Range); 
	

	FHitResult hitResult;

	// ECC_GameTraceChannel1 is only for the player
	ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1);
	
	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(),startLocation,endLocation,PunchRadius,TraceChannel,false,
		TArray<AActor*>(), // we don't need to ignore anything since the trace channel is dedicated only to the player
		EDrawDebugTrace::None,
		hitResult,
		true
		);
	
	if (bHit && hitResult.GetActor())
	{
		AActor* hitActor = hitResult.GetActor();
		// apply damage 
		UGameplayStatics::ApplyDamage(hitActor, Damage, Controller, ownerPawn, UDamageType::StaticClass());
		// KNOCKBACK !! 
		if (APlayerCharacter* targetCharacter = Cast<APlayerCharacter>(hitActor))
		{
			targetCharacter->LaunchCharacter(forwardLocation*KnockbackStrength, true, false);
		}
	}
}

// Sets default values
APunch::APunch()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APunch::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APunch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

