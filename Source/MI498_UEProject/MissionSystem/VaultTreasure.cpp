#include "VaultTreasure.h"

void AVaultTreasure::BeginPlay()
{
	Super::BeginPlay();
    
	LootToShrink.Empty();

	// Iterate all child actor components
	TArray<UChildActorComponent*> Components;
	GetComponents<UChildActorComponent>(Components);

	/// Loop through all child components
	for (UChildActorComponent* Comp : Components)
	{
		if (!Comp) continue;

		/// If this child is a loot peice add it to loot to shrink
		AActor* ChildActor = Comp->GetChildActor();
		if (ChildActor && ChildActor->ActorHasTag(FName("LootPiece")))
		{
			LootToShrink.Add(ChildActor);
		}
	}
}
