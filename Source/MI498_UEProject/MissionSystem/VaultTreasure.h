#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VaultTreasure.generated.h"

UCLASS()
class MI498_UEPROJECT_API AVaultTreasure : public AActor
{
	GENERATED_BODY()

public:
	/// The loot piles to shrink when the player is in the vault
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> LootToShrink;
	
	/// The original scale of all the loot objects
	TArray<FVector> LootOriginalScale;
	
	/// The original position of all the loot objects
	TArray<FVector> LootOriginalPosition;
	
	virtual void BeginPlay() override;
};
