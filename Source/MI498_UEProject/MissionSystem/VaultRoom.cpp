#include "VaultRoom.h"

#include "Components/BoxComponent.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"

AVaultRoom::AVaultRoom()
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

void AVaultRoom::InVaultStatusChange(bool Status)
{
	OnVaultDoorInteract.Broadcast(Status);
}

void AVaultRoom::BeginPlay()
{
	Super::BeginPlay();

	RoomTrigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&AVaultRoom::OnRoomBeginOverlap
	);

	RoomTrigger->OnComponentEndOverlap.AddDynamic(
		this,
		&AVaultRoom::OnRoomEndOverlap
	);
}

void AVaultRoom::OnRoomBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (Cast<APlayerCharacter>(OtherActor))
	{
		InVaultStatusChange(true);
	}
}

void AVaultRoom::OnRoomEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (Cast<APlayerCharacter>(OtherActor))
	{
		InVaultStatusChange(false);
	}
}
