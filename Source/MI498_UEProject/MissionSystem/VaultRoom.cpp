#include "VaultRoom.h"

#include "Components/BoxComponent.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"

AVaultRoom::AVaultRoom()
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

void AVaultRoom::InVaultStatusChange(bool Status) const
{
	/// Broadcast status change to any listeners
	OnVaultDoorInteract.Broadcast(Status);
}

void AVaultRoom::BeginPlay()
{
	Super::BeginPlay();

	/// Bind begin-overlap event to handler
	RoomTrigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&AVaultRoom::OnRoomBeginOverlap
	);

	/// Bind end-overlap event to handler
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
	/// Notifiy listeners if the player entered the vault
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
	/// Notifiy listeners if the player exited the vault
	if (Cast<APlayerCharacter>(OtherActor))
	{
		InVaultStatusChange(false);
	}
}
