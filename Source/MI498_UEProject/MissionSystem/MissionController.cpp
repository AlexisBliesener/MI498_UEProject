#include "MissionController.h"
#include "BombPiece.h"
#include "ExitCannonComponent.h"
#include "ExitPlatform.h"
#include "NavigationSystem.h"
#include "PlantedBomb.h"
#include "VaultDoor.h"
#include "VaultRoom.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "MI498_UEProject/ScoringSystem/ScoringManager.h"


AMissionController::AMissionController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMissionController::BeginPlay()
{
	Super::BeginPlay();
	
	ScoringManager = GetGameInstance()->GetSubsystem<UScoringManager>();
	
	/// Set how many bomb peices are needed to complete stage one
	NeededBombPieces = BombPieces.Num();
	
	OnMissionStarted();
	
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
	
	/// Bind to exit cannon component events
	UExitCannonComponent* CannonComp = ExitCannon->FindComponentByClass<UExitCannonComponent>();
	
	CannonComp->OnNearExitCannon.AddDynamic(
		this,
		&AMissionController::HandleOnNearExitCannon);
	
	CannonComp->OnShotFromCannon.AddDynamic(
		this,
		&AMissionController::HandleOnNearExitCannon);
	
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
	if (APlayerController* playrController = GetWorld()->GetFirstPlayerController())
	{
		if (playrController->WasInputKeyJustPressed(EKeys::F7))
		{

			ExplodeVaultDoor();
		}
		if (playrController->WasInputKeyJustPressed(EKeys::F10))
		{
			UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			NavSys->AddNavigationBuildLock(1);
		}
		if (playrController->WasInputKeyJustPressed(EKeys::F9))
		{
			UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			NavSys->RemoveNavigationBuildLock(1);
		}
	}
}

void AMissionController::HandleBombPieceCollected()
{
	/// Award score for collecting bomb piece
	ScoringManager->AddBombPieceScore();
	BombPiecesCollected++;
	
	/// Trigger contextual VA based on progress
	if (BombPiecesCollected == 1)
	{
		OnFirstBombPieceCollected();
	}
	else if (BombPiecesCollected == 2)
	{
		OnSecondBombPieceCollected();
	}
	else
	{
		OnThirdBombPieceCollected();
	}
}

void AMissionController::HandleVaultDoorInteract()
{
	/// Only allow bomb planting during Stage Two
	if (CurrentState == EMissionState::StageTwo)
	{
		OnBombPlanted();
		
		PlantedBomb->BombAppear();
		
		/// Delay vault explosion
		GetWorldTimerManager().SetTimer(
			MissionTimerHandle,
			this,
			&AMissionController::ExplodeVaultDoor,
			7,
			false);
	}
}

void AMissionController::ExplodeVaultDoor()
{
	/// Trigger explosion effects and scoring
	PlantedBomb->BombExplode();
	OnBombExplode();
	ScoringManager->AddOpenVaultScore();
	StageTwoFinish(true);
	VaultDoor->Destroy();

	/// Start falling ships
	for (TObjectPtr<AShip> ship : Ships)
	{
		ship->StartFalling();
	}
}

void AMissionController::HandleOnEnterExitPlatform()
{
	/// Level completion condition (Stage Three)
	if (CurrentState == EMissionState::StageThree)
	{
		ScoringManager->AddFinishLevelScore();
		StageThreeFinish(true);
	}
}

void AMissionController::HandleInVaultStatusChange(bool Status)
{
	if (Status)
	{
		if (!bNearVaultVaLinePlayed)
		{
			bNearVaultVaLinePlayed = true;
			OnNearVault();
		}
		
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
		
		if (!bOnLeaveVaultVaLinePlayed && CurrentState == EMissionState::StageThree)
		{
			bOnLeaveVaultVaLinePlayed = true;
			OnLeaveVault();
		}
	}
}

void AMissionController::HandleOnNearExitCannon()
{
	if (!bOnNearExitCannonVaLinePlayed && CurrentState == EMissionState::StageThree)
	{
		bOnNearExitCannonVaLinePlayed = true;
		OnNearExitCannon();
	}
}

void AMissionController::HandleOnShotFromExitCannon()
{
	if (CurrentState == EMissionState::StageThree)
	{
		OnShotFromExitCannon();
	}
}

void AMissionController::SecondInVault()
{
	if (CurrentState == EMissionState::StageThree)
	{
		ScoringManager->AddVaultSecScore();
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

float AMissionController::GetRemainingMissionTime() const
{
	if (GetWorldTimerManager().GetTimerRemaining(MissionTimerHandle) < 0)
	{
		return 0;
	}
	return GetWorldTimerManager().GetTimerRemaining(MissionTimerHandle);
}
