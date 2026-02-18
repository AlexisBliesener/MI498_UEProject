// Fill out your copyright notice in the Description page of Project Settings.


#include "ExitCannonComponent.h"

#include "Components/BoxComponent.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"


// Sets default values for this component's properties
UExitCannonComponent::UExitCannonComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	/// Create the box component used as the trigger volume
	NearObjectTrigger = CreateDefaultSubobject<UBoxComponent>("RoomTrigger");

	// Set default size
	NearObjectTrigger->SetBoxExtent(FVector(500, 500, 300));

	// Make it a trigger
	NearObjectTrigger->SetCollisionProfileName(TEXT("Trigger"));
	NearObjectTrigger->SetGenerateOverlapEvents(true);
}


void UExitCannonComponent::NearExitCannon() const
{
	/// Notify any listeners that the player is near the cannon
	OnNearExitCannon.Broadcast();
}

void UExitCannonComponent::ShotFromCannon() const
{
	OnShotFromCannon.Broadcast();
}

void UExitCannonComponent::BeginPlay()
{
	Super::BeginPlay();

	/// Bind the overlap begin event to handler function
	NearObjectTrigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&UExitCannonComponent::OnBeginOverlap
	);
	
}

void UExitCannonComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/// Fire near exit cannon event when the player enters the trigger
	if (Cast<APlayerCharacter>(OtherActor))
	{
		NearExitCannon();
	}
}

