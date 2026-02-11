#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BombPiece.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBombPieceCollected);

UCLASS()
class MI498_UEPROJECT_API ABombPiece : public AActor
{

public:
	FOnBombPieceCollected OnBombPieceCollected;
	
	UFUNCTION(BlueprintCallable)
	void BombPieceCollected();

protected:

private:
	
	GENERATED_BODY()

};
