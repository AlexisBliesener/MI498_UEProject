#pragma once
#include "CoreMinimal.h"
#include "../Ships/Ship.h"
#include "GameFramework/Actor.h"
#include "MissionController.generated.h"

class APlantedBomb;
class UExitCannonComponent;
class UScoringManager;
class AEnemyBase;
class AExitPlatform;
class AVaultRoom;
class AVaultDoor;
class ABombPiece;

/// Enum representing the current mission stage
/// Controls progression logic and win/fail conditions
enum class EMissionState : uint8
{
	StageOne UMETA(DisplayName = "StageOne"),
	StageTwo UMETA(DisplayName = "StageTwo"),
	StageThree UMETA(DisplayName = "StageThree")
};

/// Central mission flow controller
/// Coordinates objectives, timers, enemy waves, and stage transitions
UCLASS(Blueprintable, BlueprintType)
class MI498_UEPROJECT_API AMissionController : public AActor
{
public:
	AMissionController();
	
	virtual void BeginPlay() override;
	
	/// Bomb piece actors required for Stage One completion
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<ABombPiece>> BombPieces;
	
	/// Vault door actor used during Stage Two
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AVaultDoor> VaultDoor;
	
	/// Vault room trigger actor used during Stage Three
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AVaultRoom> VaultRoom;
	
	/// Exit platform actor used to complete Stage Three
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AExitPlatform> ExitPlatform;
	
	/// Enemy class to spawn for combat waves
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemyBase> AverageEnemy;
	
	/// World actors used as enemy spawn locations
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> EnemySpawnPoints;
	
	/// Actor containing the exit cannon component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> ExitCannon;
	
	/// Bomb that will explode the vault door
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlantedBomb> PlantedBomb;
	
	/// Array of ships in the level
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<AShip>> Ships;
	
	/// A Blueprintable function that will be called when the mission starts
	UFUNCTION(BlueprintImplementableEvent)
	void OnMissionStarted();
	
	/// Fired when player approaches the vault
	UFUNCTION(BlueprintImplementableEvent)
	void OnNearVault();
	
	/// Fired when player leaves the vault
	UFUNCTION(BlueprintImplementableEvent)
	void OnLeaveVault();
	
	/// Fired when the vault bomb explodes
	UFUNCTION(BlueprintImplementableEvent)
	void OnBombExplode();
	
	/// Fired when the bomb is planted
	UFUNCTION(BlueprintImplementableEvent)
	void OnBombPlanted();

	/// Fired when first bomb piece is collected
	UFUNCTION(BlueprintImplementableEvent)
	void OnFirstBombPieceCollected();

	/// Fired when second bomb piece is collected
	UFUNCTION(BlueprintImplementableEvent)
	void OnSecondBombPieceCollected();

	/// Fired when third bomb piece is collected
	UFUNCTION(BlueprintImplementableEvent)
	void OnThirdBombPieceCollected();

	/// Fired when player approaches exit cannon
	UFUNCTION(BlueprintImplementableEvent)
	void OnNearExitCannon();

	/// Fired when player is launched from exit cannon
	UFUNCTION(BlueprintImplementableEvent)
	void OnShotFromExitCannon();
	
	/// How much additional time the player will get to complete stage three, in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int StageThreeAdditionalTime = 180;
	
	/// Time that must be spent in the vault to gather one loot
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TimeInVaultToCollectSingleLoot = 1;
	
	/// Time in between spawning waves of enemies in the vault room
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TimeInBetweenSpawningEnemyWaves = 15;
	
	/// Get remaining time
	UFUNCTION(BlueprintCallable, Category = "Mission|Timer")
	float GetRemainingMissionTime() const;
	
protected:
	virtual void Tick(float DeltaSeconds) override;
	
	/// Blueprint event fired when the mission fails
	UFUNCTION(BlueprintImplementableEvent)
	void OnFailedMission();
	
	/// Blueprint event fired when the mission succeeds
	UFUNCTION(BlueprintImplementableEvent)
	void OnSucceedMission();
	
private:
	/// Delegate handler for bomb piece collection
	UFUNCTION()
	void HandleBombPieceCollected();
	
	/// Delegate handler for vault door interaction
	UFUNCTION()
	void HandleVaultDoorInteract();
	
	/// Delegate handler for exit platform entry
	UFUNCTION()
	void HandleOnEnterExitPlatform();
	
	/// Delegate handler for vault room enter/exit status
	UFUNCTION()
	void HandleInVaultStatusChange(bool Status);
	
	/// Handles player entering exit cannon trigger
	UFUNCTION()
	void HandleOnNearExitCannon();
	
	/// Handles player being launched from exit cannon
	UFUNCTION()
	void HandleOnShotFromExitCannon();
	
	/// Called to explode the vault door
	void ExplodeVaultDoor();
	
	/// Called once per second while player remains inside the vault
	void SecondInVault();

	/// Spawns enemies at all configured spawn points
	void SpawnEnemies();
	
	/// Handles completion or timeout of Stage One
	void StageOneFinish(bool Result);

	/// Handles completion or timeout of Stage Two
	void StageTwoFinish(bool Result);

	/// Handles completion or timeout of Stage Three
	void StageThreeFinish(bool Result);
	
	/// How many bomb pieces are needed to make the bomb
	int NeededBombPieces;
	
	/// Number of bomb pieces collected so far
	int BombPiecesCollected = 0;
	
	/// Timer handle for tracking time spent inside the vault
	FTimerHandle InVaultTimerHandle;
	
	/// Timer handle for when the bomb is going to explode
	FTimerHandle BombExplosionTimerHandle;
	
	/// Seconds the player has remained inside the vault
	int SecondsInVault = 0;
	
	/// Main mission stage timer handle
	FTimerHandle MissionTimerHandle;
	
	/// Repeating timer handle for enemy wave spawning
	FTimerHandle EnemyWaveSpawnerTimerHandle;
	
	/// Current mission stage state
	EMissionState CurrentState = EMissionState::StageOne;
	
	/// If the near vault va line has played
	bool bNearVaultVaLinePlayed = false;
	
	/// If the on leave vault va line has played
	bool bOnLeaveVaultVaLinePlayed = false;
	
	/// If the near exit cannon VA line has played
	bool bOnNearExitCannonVaLinePlayed = false;
	
	/// Scoring system instance
	UPROPERTY()
	UScoringManager * ScoringManager = nullptr;
	
	GENERATED_BODY()
};
