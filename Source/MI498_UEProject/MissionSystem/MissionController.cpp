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
	
	FTimerDelegate delegate;
	delegate.BindUObject(this, &AMissionController::StageOneFinish, false);
	
	// Start Stage One timer
	GetWorldTimerManager().SetTimer(
		MissionTimerHandle,
		delegate,
		240,
		false
		);
	
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
	
	VaultDoor->OnVaultDoorInteract.AddDynamic(
		this,
		&AMissionController::HandleVaultDoorOpen);
	
	VaultRoom->OnVaultDoorInteract.AddDynamic(
		this,
		&AMissionController::HandleInVaultStatusChange);
	
	ExitPlatform->OnEnterExitPlatform.AddDynamic(
		this,
		&AMissionController::HandleOnEnterExitPlatform);
}

void AMissionController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UE_LOG(LogTemp, Log, TEXT("Timer: %f" ), GetWorldTimerManager().GetTimerRemaining(MissionTimerHandle));
	
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
	UE_LOG(LogTemp, Log, TEXT("Bomb piece triggered"));
	BombPiecesCollected++;
}

void AMissionController::HandleVaultDoorOpen()
{
	if (CurrentState == EMissionState::StageTwo)
	{
		StageTwoFinish(true);
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
		UE_LOG(LogTemp, Log, TEXT("Player entered vault room"));
		GetWorldTimerManager().SetTimer(
			InVaultTimerHandle,
			this,
			&AMissionController::SecondInVault,
			1.0f,
			true   // repeat every second
			);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Player left vault room"));
		GetWorldTimerManager().ClearTimer(InVaultTimerHandle);
	}
}

void AMissionController::SecondInVault()
{
	if (CurrentState == EMissionState::StageThree)
	{
		SecondsInVault++;
		UE_LOG(LogTemp, Log, TEXT("Sec in vault: %d"), SecondsInVault);
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
		UE_LOG(LogTemp, Log, TEXT("Stage 2 Start"));
		
		/// Start Stage 2
		CurrentState = EMissionState::StageTwo;
		
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
		UE_LOG(LogTemp, Log, TEXT("Stage 3 Start"));
		
		
		/// Start Stage 3
		CurrentState = EMissionState::StageThree;
		
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
		
		SpawnEnemies();
		
		/// Start enemy wave timer
		GetWorldTimerManager().SetTimer(
			EnemyWaveSpawnerTimerHandle,
			this,
			&AMissionController::SpawnEnemies,
			15,
			true   // repeat every second
			);
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
		OnSuccedMission();
	}
	else
	{
		OnFailedMission();
	}
}
