#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"
#include "ScoringManager.generated.h"

class APlayerCharacter;
class AEnemyBase;
class AWeaponBase;

/// Enemy Types Defines all supported enemy categories for scoring
UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	AverageEnemy UMETA(DisplayName="AverageEnemy"),
	Brute UMETA(DisplayName="Brute"),
	Swinger UMETA(DisplayName="Swinger")
};

/// Kill Types Defines what caused the enemy death
UENUM(BlueprintType)
enum class EKillType : uint8
{
	None UMETA(DisplayName="None"),
	Blunderbuss UMETA(DisplayName="Blunderbuss"),
	Sword UMETA(DisplayName="Sword"),
	HarpoonGun UMETA(DisplayName="HarpoonGun"),
	Barrel UMETA(DisplayName="Barrel")
};

/// Scoring Manager
/// Game-wide scoring system stored in GameInstance
UCLASS()
class MI498_UEPROJECT_API UScoringManager : public UGameInstanceSubsystem, public FTickableGameObject
{
public:

	/// Returns the actual accumulated score
	int GetScore() const { return Score; }

	/// Adds score for collecting a bomb piece
	void AddBombPieceScore() { Score += BombPieceScore; }

	/// Adds score for opening vault and updates global multiplier
	void AddOpenVaultScore()
	{
		Score += OpenVaultScore;
		SetGlobalScoreMult(AfterBombPlantGlobalMult);
	}

	/// Adds score when finishing the level
	void AddFinishLevelScore() { Score += FinishLevelScore; }

	/// Adds score for each vault second survived
	void AddVaultSecScore() { Score += VaultSecScore; }
	
	/// Sets airborne state (used for airtime scoring & midair bonuses)
	void SetInAir(bool val) { bInAir = val; }
	
	/// Generic score adder accessible from Blueprint
	UFUNCTION(BlueprintCallable)
	void AddGenericScore(int Amount) { Score += Amount * GlobalScoreMult; }

	/// Sets the global score multiplier
	void SetGlobalScoreMult(const int Val) { GlobalScoreMult = Val; }

	/// Calculates and applies score for killing an enemy
	void AddKillEnemyScore(EEnemyType Killed, EKillType KilledBy);

	/// Score awarded for collecting a single bomb piece
	UPROPERTY(EditAnywhere)
	int BombPieceScore = 5000;

	/// Score awarded when the player opens the vault
	UPROPERTY(EditAnywhere)
	int OpenVaultScore = 10000;

	/// Bonus score awarded upon completing the level
	UPROPERTY(EditAnywhere)
	int FinishLevelScore = 10000;

	/// Score awarded per second while defending the vault
	UPROPERTY(EditAnywhere)
	int VaultSecScore = 500;

	/// Base score awarded for killing a standard enemy
	UPROPERTY(EditAnywhere)
	int AverageEnemyKillScore = 500;

	/// Base score awarded for killing a Brute enemy
	UPROPERTY(EditAnywhere)
	int BruteKillScore = 1000;

	/// Base score awarded for killing a Swinger enemy
	UPROPERTY(EditAnywhere)
	int SwingerKillScore = 1000;

	/// Score added per second while airborne
	UPROPERTY(EditAnywhere)
	int AirtimeScore = 100;

	/// Flat bonus added if a kill happens midair
	UPROPERTY(EditAnywhere)
	int InAirKillBonus = 100;

	/// Multiplier applied after bomb plant event
	UPROPERTY(EditAnywhere)
	float AfterBombPlantGlobalMult = 2;

	/// Interval between UI score increments
	UPROPERTY(EditAnywhere)
	float SecToUpdateOnScreenScore = 0.01;

	/// Interval between airtime score additions
	UPROPERTY(EditAnywhere)
	float SecToAddAirtime = 1;

	/// Multiplier applied for midair kills
	UPROPERTY(EditAnywhere)
	float MidairKillModifier = 1.5;

	/// Amount added to combo multiplier when switching weapons
	UPROPERTY(EditAnywhere)
	float ComboKillModifier = 0.2;

	/// Multiplier applied to barrel kills
	UPROPERTY(EditAnywhere)
	float BombBarrelKillModifier = 1.5;

protected:
	virtual void Tick(float DeltaTime) override;

	/// Required for FTickableGameObject
	virtual TStatId GetStatId() const override;

	/// Determines whether subsystem should tick
	virtual bool IsTickable() const override;

private:

	/// Adds airtime score while airborne
	void AddAirtimeScore();

	/// Smoothly increments on-screen score toward real score
	void UpdateOnScreenScore();

	/// Current combo multiplier value
	float CurrentComboKillMult = 1;
	
	/// Score shown on UI
	int OnScreenScore = 0;

	/// True accumulated score value
	int Score = 0;

	/// Global score multiplier affecting most score additions
	float GlobalScoreMult = 1;
	
	/// Whether player is currently airborne
	bool bInAir = false;

	/// Internal flag to prevent restarting airtime timer repeatedly
	bool bAirborneSet = false;
	
	/// Indicates whether UI score update timer is running
	bool bOnScreenScoreUpdating = false;
	
	/// Timer handle for airtime scoring
	FTimerHandle AirtimeTimerHandle;

	/// Timer handle for smooth UI score updating
	FTimerHandle OnScreenScoreTimerHandle;

	/// Stores the previous kill type for combo logic comparison
	UPROPERTY()
	EKillType LastKilledWith = EKillType::None;

	GENERATED_BODY()
};
