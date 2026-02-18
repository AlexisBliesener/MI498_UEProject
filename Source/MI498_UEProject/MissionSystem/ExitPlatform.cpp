#include "ExitPlatform.h"
#include "Components/BoxComponent.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"

AExitPlatform::AExitPlatform()
{
	/// Create the box component used as the trigger volume
	RoomTrigger = CreateDefaultSubobject<UBoxComponent>("RoomTrigger");
	RootComponent = RoomTrigger;

	// Set default size
	RoomTrigger->SetBoxExtent(FVector(500, 500, 300));

	// Make it a trigger
	RoomTrigger->SetCollisionProfileName(TEXT("Trigger"));
	RoomTrigger->SetGenerateOverlapEvents(true);
}

void AExitPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	/// Bind the overlap begin event to handler function
	RoomTrigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&AExitPlatform::OnRoomBeginOverlap
	);
	
}

void AExitPlatform::EnterExitPlatform() const
{
	/// Notify any listeners that the platform was entered
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
	/// Fire exit platformed entered if the player entered the platform
	if (Cast<APlayerCharacter>(OtherActor))
	{
		EnterExitPlatform();
	}
}
