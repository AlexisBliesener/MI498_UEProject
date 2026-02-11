#include "ExitPlatform.h"

#include "Components/BoxComponent.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"

AExitPlatform::AExitPlatform()
{
	PrimaryActorTick.bCanEverTick = false;

	RoomTrigger = CreateDefaultSubobject<UBoxComponent>("RoomTrigger");
	RootComponent = RoomTrigger;

	// Set default size (rectangle prism)
	RoomTrigger->SetBoxExtent(FVector(500, 500, 300));

	// Make it a trigger
	RoomTrigger->SetCollisionProfileName(TEXT("Trigger"));
	RoomTrigger->SetGenerateOverlapEvents(true);
}

void AExitPlatform::BeginPlay()
{
	Super::BeginPlay();

	RoomTrigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&AExitPlatform::OnRoomBeginOverlap
	);
	
}

void AExitPlatform::EnterExitPlatform()
{
	OnEnterExitPlatform.Broadcast();
}

void AExitPlatform::OnRoomBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (Cast<APlayerCharacter>(OtherActor))
	{
		EnterExitPlatform();
	}
}
