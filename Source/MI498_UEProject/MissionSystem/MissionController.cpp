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
	
	// Start Stage One timer
	FTimerDelegate delegate;
	delegate.BindUObject(this, &AMissionController::StageOneFinish, false);
	GetWorldTimerManager().SetTimer(
		MissionTimerHandle,
		delegate,
		240,
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
		if (BombPiecesCollected == 3)
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
		/// Start repeating 1-second timer while inside vault
		GetWorldTimerManager().SetTimer(
			InVaultTimerHandle,
			this,
			&AMissionController::SecondInVault,
			1.0f,
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

void AMissionController::StageOneFinish(bool Result)
{
	if (Result)
	{
		/// Start Stage 2
		CurrentState = EMissionState::StageTwo;
		
		/// Extend remaining mission time by 120 seconds
		float seconds = GetWorldTimerManager().GetTimerRemaining(MissionTimerHandle);
		seconds += 120;
		
		GetWorldTimerManager().ClearTimer(MissionTimerHandle);
		
		FTimerDelegate delegate;
		delegate.BindUObject(this, &AMissionController::StageTwoFinish, false);
		GetWorldTimerManager().SetTimer(
			MissionTimerHandle,
			delegate,
			seconds,
			false
			);
	}
	else
	{
		OnFailedMission();
	}
}

void AMissionController::StageTwoFinish(bool Result)
{
	if (Result)
	{
		/// Start Stage 3
		CurrentState = EMissionState::StageThree;
		
		/// Extend remaining mission time by 60 seconds
		float seconds = GetWorldTimerManager().GetTimerRemaining(MissionTimerHandle);
		seconds += 60;
		
		GetWorldTimerManager().ClearTimer(MissionTimerHandle);
		
		FTimerDelegate delegate;
		delegate.BindUObject(this, &AMissionController::StageThreeFinish, false);
		
		GetWorldTimerManager().SetTimer(
			MissionTimerHandle,
			delegate,
			seconds,
			false
			);
		
		/// Spawn initial enemy wave
		SpawnEnemies();
		
		/// Start repeating enemy wave spawner timer
		GetWorldTimerManager().SetTimer(
			EnemyWaveSpawnerTimerHandle,
			this,
			&AMissionController::SpawnEnemies,
			15,
			true);
	}
	else
	{
		OnFailedMission();
	}
}

void AMissionController::StageThreeFinish(bool Result)
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
