#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "ScoringManager.generated.h"


class AEnemyBase;
class AWeaponBase;

enum class EEnemyType : uint8
{
	AverageEnemy UMETA(DisplayName="AverageEnemy"),
	Brute UMETA(DisplayName="Brute"),
	Swinger UMETA(DisplayName="Swinger")
};

UENUM(BlueprintType)
enum class EKillType : uint8
{
	None UMETA(DisplayName="None"),
	Blunderbuss UMETA(DisplayName="Blunderbuss"),
	Sword UMETA(DisplayName="Sword"),
	HarpoonGun UMETA(DisplayName="HarpoonGun"),
	Barrel UMETA(DisplayName="Barrel")
};

UCLASS()
class MI498_UEPROJECT_API UScoringManager : public UGameInstanceSubsystem, public FTickableGameObject
{
public:
	int GetScore() const { return Score; }

	void AddBombPieceScore() { Score += BombPieceScore; }

	void AddOpenVaultScore()
	{
		Score += OpenVaultScore;
		SetGlobalScoreMult(AfterBombPlantGlobalMult);
	}

	void AddFinishLevelScore() { Score += FinishLevelScore; }
	void AddVaultSecScore() { Score += VaultSecScore; }
	void AddKillEnemyScore(const EEnemyType Killed, const EKillType KilledWith);

	void SetGlobalScoreMult(const int Val) { GlobalScoreMult = Val; }

	UPROPERTY(EditAnywhere)
	int BombPieceScore = 5000;

	UPROPERTY(EditAnywhere)
	int OpenVaultScore = 10000;

	UPROPERTY(EditAnywhere)
	int FinishLevelScore = 10000;

	UPROPERTY(EditAnywhere)
	int VaultSecScore = 500;
	UPROPERTY(EditAnywhere)
	int AverageEnemyKillScore = 500;
	UPROPERTY(EditAnywhere)
	int BruteKillScore = 1000;
	UPROPERTY(EditAnywhere)
	int SwingerKillScore = 1000;

	UPROPERTY(EditAnywhere)
	float AfterBombPlantGlobalMult = 2;


	UPROPERTY(EditAnywhere)
	float MidairKillModifier = 1.5;

	UPROPERTY(EditAnywhere)
	float ComboKillModifier = 0.2;

	UPROPERTY(EditAnywhere)
	float BombBarrelKillModifier = 1.5;

protected:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;

private:
	float CurrentComboKillMult = 1;

	int OnScreenScore;
	int Score = 0;
	float GlobalScoreMult;

	UPROPERTY()
	EKillType LastKilledWith = EKillType::None;

	GENERATED_BODY()
};
