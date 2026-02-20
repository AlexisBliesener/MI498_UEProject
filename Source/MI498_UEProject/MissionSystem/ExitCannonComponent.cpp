#include "ExitCannonComponent.h"
#include "Components/BoxComponent.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"


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

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UBoxComponent* Trigger = Owner->FindComponentByClass<UBoxComponent>();
	if (!Trigger)
	{
		return;
	}

	Trigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&UExitCannonComponent::OnBeginOverlap
	);
}

void UExitCannonComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AActor* Owner = GetOwner())
	{
		if (UBoxComponent* Trigger = Owner->FindComponentByClass<UBoxComponent>())
		{
			Trigger->OnComponentBeginOverlap.RemoveDynamic(
				this,
				&UExitCannonComponent::OnBeginOverlap
			);
		}
	}

	Super::EndPlay(EndPlayReason);
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

