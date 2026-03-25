#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BombPiece.generated.h"

/// Dynamic multicast delegate that is broadcast when a bomb piece is collected
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBombPieceCollected, int32, index);

/// Actor representing a collectible bomb piece in the world
/// Responsible for notifying listeners when it has been collected
UCLASS()
class MI498_UEPROJECT_API ABombPiece : public AActor
{

public:
	/// Event triggered when this bomb piece is collected
	FOnBombPieceCollected OnBombPieceCollected;
	
	/// Called to mark this bomb piece as collected
	/// Broadcasts the OnBombPieceCollected delegate
	UFUNCTION(BlueprintCallable)
	void BombPieceCollected(int32 index);

protected:

private:
	
	GENERATED_BODY()

};
