#include "BombPiece.h"

void ABombPiece::BombPieceCollected()
{
	OnBombPieceCollected.Broadcast();
	Destroy();
}
