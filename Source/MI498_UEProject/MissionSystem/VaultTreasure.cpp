#include "VaultTreasure.h"

void AVaultTreasure::BeginPlay()
{
	Super::BeginPlay();
    
	LootToShrink.Empty();

	// Iterate all child actor components
	TArray<UChildActorComponent*> Components;
	GetComponents<UChildActorComponent>(Components);

	for (UChildActorComponent* Comp : Components)
	{
		if (!Comp) continue;

		AActor* ChildActor = Comp->GetChildActor();
		if (ChildActor && ChildActor->ActorHasTag(FName("LootPiece")))
		{
			LootToShrink.Add(ChildActor);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Found %d loot pieces!"), LootToShrink.Num());
}
