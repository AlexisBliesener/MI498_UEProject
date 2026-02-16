#include "ScoringManager.h"

#include "MI498_UEProject/Weapons/WeaponBase.h"

void UScoringManager::AddKillEnemyScore(EEnemyType Killed, EKillType KilledBy)
{
	int add = 0;

	switch (Killed)
	{
	case EEnemyType::AverageEnemy:
		add = AverageEnemyKillScore;
		break;

	case EEnemyType::Brute:
		add = BruteKillScore;
		break;

	case EEnemyType::Swinger:
		add = SwingerKillScore;
		break;

	default:
		break;
	}


	if (LastKilledWith != KilledBy && KilledBy != EKillType::Barrel)
	{
		CurrentComboKillMult += ComboKillModifier;
		add *= CurrentComboKillMult;
	}
	else if (KilledBy != EKillType::Barrel)
	{
		CurrentComboKillMult = 1;
	}
	LastKilledWith = KilledBy;
	
	add *= GlobalScoreMult;
	
	if (KilledBy == EKillType::Barrel)
	{
		add *= BombBarrelKillModifier;
	}
	
	Score += add;
}


void UScoringManager::Tick(float DeltaTime)
{
	UE_LOG(LogTemp, Log, TEXT("Score: %d"), Score);
}

TStatId UScoringManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UScoringManager, STATGROUP_Tickables);
}

bool UScoringManager::IsTickable() const
{
	return FTickableGameObject::IsTickable();
}
