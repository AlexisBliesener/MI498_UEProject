#include "BombPiece.h"

void ABombPiece::BombPieceCollected(int32 index)
{
	/// Broadcast the collection event then destory the bomb
	OnBombPieceCollected.Broadcast(index);
	Destroy();
}
