#include "MissionController.h"
#include "BombPiece.h"
#include "ExitPlatform.h"
#include "VaultDoor.h"
#include "VaultRoom.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"


AMissionController::AMissionController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMissionController::BeginPlay()
{
	Super::BeginPlay();
	
	/// Set how many bomb peices are needed to complete stage one
	NeededBombPieces = BombPieces.Num();
	
	// Start Stage One timer
	FTimerDelegate delegate;
	delegate.BindUObject(this, &AMissionController::StageOneFinish, false);
	GetWorldTimerManager().SetTimer(
		MissionTimerHandle,
		delegate,
		StageOneTimeLimit,
		false
		);
	
	/// Bind to all bomb piece collected events
	for (ABombPiece* Piece : BombPieces)
	{
		if (Piece)
		{
			Piece->OnBombPieceCollected.AddDynamic(
				this,
				&AMissionController::HandleBombPieceCollected
			);
		}
	}
	
	/// Bind to vault door interaction event
	VaultDoor->OnVaultDoorInteract.AddDynamic(
		this,
		&AMissionController::HandleVaultDoorInteract);
	
	/// Bind to vault room enter/exit status event
	VaultRoom->OnVaultDoorInteract.AddDynamic(
		this,
		&AMissionController::HandleInVaultStatusChange);
	
	/// Bind to exit platform enter event
	ExitPlatform->OnEnterExitPlatform.AddDynamic(
		this,
		&AMissionController::HandleOnEnterExitPlatform);
}

void AMissionController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	/// Check Stage One completion condition
	if (CurrentState == EMissionState::StageOne)
	{
		if (BombPiecesCollected == NeededBombPieces)
		{
			StageOneFinish(true);
		}
	}
}

void AMissionController::HandleBombPieceCollected()
{
	BombPiecesCollected++;
}

void AMissionController::HandleVaultDoorInteract()
{
	if (CurrentState == EMissionState::StageTwo)
	{
		StageTwoFinish(true);
		VaultDoor->Destroy();
	}
}

void AMissionController::HandleOnEnterExitPlatform()
{
	if (CurrentState == EMissionState::StageThree)
	{
		StageThreeFinish(true);
	}
}

void AMissionController::HandleInVaultStatusChange(bool Status)
{
	if (Status)
	{
		/// Start repeating timer while inside vault
		GetWorldTimerManager().SetTimer(
			InVaultTimerHandle,
			this,
			&AMissionController::SecondInVault,
			TimeInVaultToCollectSingleLoot,
			true);
	}
	else
	{
		/// Stop vault timer when player exits
		GetWorldTimerManager().ClearTimer(InVaultTimerHandle);
	}
}

void AMissionController::SecondInVault()
{
	if (CurrentState == EMissionState::StageThree)
	{
		SecondsInVault++;
	}
}

void AMissionController::SpawnEnemies()
{
	if (!AverageEnemy) return;

	for (AActor* SpawnPoint : EnemySpawnPoints)
	{
		if (!SpawnPoint) continue;

		FVector Location = SpawnPoint->GetActorLocation();
		FRotator Rotation = SpawnPoint->GetActorRotation();

		/// Spawn enemy instance
		GetWorld()->SpawnActor<AEnemyBase>(
			AverageEnemy,
			Location,
			Rotation
		);
	}
}

void AMissionController::StageOneFinish(const bool Result)
{
	if (Result)
	{
		/// Start Stage 2
		CurrentState = EMissionState::StageTwo;
		
		/// Extend remaining mission time by StageTwoAdditionalTime seconds
		float seconds = GetWorldTimerManager().GetTimerRemaining(MissionTimerHandle);
		seconds += StageTwoAdditionalTime;
		
		GetWorldTimerManager().ClearTimer(MissionTimerHandle);
		
		FTimerDelegate delegate;
		delegate.BindUObject(this, &AMissionController::StageTwoFinish, false);
		GetWorldTimerManager().SetTimer(
			MissionTimerHandle,
			delegate,
			seconds,
			false);
	}
	else
	{
		OnFailedMission();
	}
}

void AMissionController::StageTwoFinish(const bool Result)
{
	if (Result)
	{
		/// Start Stage 3
		CurrentState = EMissionState::StageThree;
		
		/// Extend remaining mission time by StageThreeAdditionalTime seconds
		float seconds = GetWorldTimerManager().GetTimerRemaining(MissionTimerHandle);
		seconds += StageThreeAdditionalTime;
		
		GetWorldTimerManager().ClearTimer(MissionTimerHandle);
		
		FTimerDelegate delegate;
		delegate.BindUObject(this, &AMissionController::StageThreeFinish, false);
		
		GetWorldTimerManager().SetTimer(
			MissionTimerHandle,
			delegate,
			seconds,
			false);
		
		/// Spawn initial enemy wave
		SpawnEnemies();
		
		/// Start repeating enemy wave spawner timer
		GetWorldTimerManager().SetTimer(
			EnemyWaveSpawnerTimerHandle,
			this,
			&AMissionController::SpawnEnemies,
			TimeInBetweenSpawningEnemyWaves,
			true);
	}
	else
	{
		OnFailedMission();
	}
}

void AMissionController::StageThreeFinish(const bool Result)
{
	if (Result)
	{
		OnSucceedMission();
	}
	else
	{
		OnFailedMission();
	}
}
