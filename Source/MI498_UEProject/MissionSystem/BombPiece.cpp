#include "BombPiece.h"

void ABombPiece::BombPieceCollected()
{
	/// Broadcast the collection event then destory the bomb
	OnBombPieceCollected.Broadcast();
	Destroy();
}
