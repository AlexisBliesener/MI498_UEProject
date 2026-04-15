#include "ScoringManager.h"

void UScoringManager::ResetScore()
{
	EscapedVal = 0;
	OpenVaultVal = 0;
	BombPiecesVal = 0;
	SideMissionVal = 0;
	VaultLootVal = 0;
	AverageEnemyKillsVal = 0;
	BruteKillsVal = 0;
	SwingerKillVal = 0;
	LootPickupVal = 0;
	BreakableWallsVal = 0;
	CurrentComboKillMult = 1;
	OnScreenScore = 0;
	Score = 0;
	GlobalScoreMult = 1;
	bInAir = false;
	bAirborneSet = false;
	bOnScreenScoreUpdating = false;
}

void UScoringManager::AddKillEnemyScore(EEnemyType Killed, EKillType KilledBy)
{
	// Base score to be calculated for this kill
	int add = 0;

	/// Determine base score based on enemy type
	switch (Killed)
	{
	case EEnemyType::AverageEnemy:
		add = Config->AverageEnemyKillScore;
		break;

	case EEnemyType::Brute:
		add = Config->BruteKillScore;
		break;

	case EEnemyType::Swinger:
		add = Config->SwingerKillScore;
		break;

	default:
		break;
	}

	/// If the player switches weapons between kills
	if (LastKilledWith != KilledBy && KilledBy != EKillType::Barrel)
	{
		CurrentComboKillMult += Config->ComboKillModifier;
		add *= CurrentComboKillMult;
	}
	/// If same weapon used consecutively, reset multiplier
	else if (KilledBy != EKillType::Barrel)
	{
		CurrentComboKillMult = 1;
	}

	// Store last kill type for next combo comparison
	LastKilledWith = KilledBy;

	/// Mid-air bonus
	if (bInAir)
	{
		add += Config->InAirKillBonus;
	}

	/// Apply global score multiplier
	add *= GlobalScoreMult;

	/// Barrel bonus multiplier
	if (KilledBy == EKillType::Barrel)
	{
		add *= Config->BombBarrelKillModifier;
	}

	/// Final score application
	Score += add;

	/// Add to specific kill enemy score total
	switch (Killed)
	{
	case EEnemyType::AverageEnemy:
		AverageEnemyKillsVal += add;
		break;

	case EEnemyType::Brute:
		BruteKillsVal += add;
		break;

	case EEnemyType::Swinger:
		SwingerKillVal += add;
		break;

	default:
		break;
	}
}

FString UScoringManager::GetRank() const
{
	if (Score < Config->CRankScore)
	{
		return TEXT("D");
	}
	if (Score < Config->BRankScore)
	{
		return TEXT("C");
	}
	if (Score < Config->ARankScore)
	{
		return TEXT("B");
	}
	if (Score < Config->SRankScore)
	{
		return TEXT("A");
	}
	return TEXT("S");
}

void UScoringManager::Tick(float DeltaTime)
{
	/// On-Screen Score Smooth Update Logic
	if (!bOnScreenScoreUpdating && OnScreenScore != Score)
	{
		bOnScreenScoreUpdating = true;
		GetWorld()->GetTimerManager().SetTimer(OnScreenScoreTimerHandle, this, &UScoringManager::UpdateOnScreenScore,
		                                       Config->SecToUpdateOnScreenScore, true);
	}
}

TStatId UScoringManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UScoringManager, STATGROUP_Tickables);
}

bool UScoringManager::IsTickable() const
{
	return FTickableGameObject::IsTickable();
}

void UScoringManager::UpdateOnScreenScore()
{
	/// Increment displayed score gradually for smooth UI effect
	OnScreenScore++;

	/// If display score catches up to real score clamp and stop updating
	if (OnScreenScore >= Score)
	{
		OnScreenScore = Score;

		GetWorld()->GetTimerManager().ClearTimer(OnScreenScoreTimerHandle);
		bOnScreenScoreUpdating = false;
	}
}
