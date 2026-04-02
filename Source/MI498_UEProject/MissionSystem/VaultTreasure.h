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
	
	virtual void BeginPlay() override;
	
};
