#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissionController.generated.h"

class AEnemyBase;
class AExitPlatform;
class AVaultRoom;
class AVaultDoor;
class ABombPiece;

enum class EMissionState : uint8
{
	StageOne UMETA(DisplayName = "StageOne"),
	StageTwo UMETA(DisplayName = "StageTwo"),
	StageThree UMETA(DisplayName = "StageThree")
};

UCLASS()
class MI498_UEPROJECT_API AMissionController : public AActor
{
public:
	AMissionController();
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<ABombPiece>> BombPieces;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AVaultDoor> VaultDoor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AVaultRoom> VaultRoom;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AExitPlatform> ExitPlatform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemyBase> AverageEnemy;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> EnemySpawnPoints;
	
protected:
	
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnFailedMission();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnSuccedMission();
	
private:
	
	UFUNCTION()
	void HandleBombPieceCollected();
	
	UFUNCTION()
	void HandleVaultDoorInteract();
	
	UFUNCTION()
	void HandleOnEnterExitPlatform();
	
	UFUNCTION()
	void HandleInVaultStatusChange(bool Status);
	
	void SecondInVault();
	void SpawnEnemies();
	
	void StageOneFinish(bool Result);
	void StageTwoFinish(bool Result);
	void StageThreeFinish(bool Result);
	
	int BombPiecesCollected = 0;
	
	FTimerHandle InVaultTimerHandle;
	
	int SecondsInVault = 0;
	
	FTimerHandle MissionTimerHandle;
	
	FTimerHandle EnemyWaveSpawnerTimerHandle;
	
	EMissionState CurrentState = EMissionState::StageOne;
	
	GENERATED_BODY()
	
};
