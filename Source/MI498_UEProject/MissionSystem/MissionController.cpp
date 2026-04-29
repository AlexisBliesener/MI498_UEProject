#include "MissionController.h"
#include "AIController.h"
#include "BombPiece.h"
#include "CloudSpawner.h"
#include "ExitCannonComponent.h"
#include "ExitPlatform.h"
#include "OutsideVaultDoor.h"
#include "PlantedBomb.h"
#include "VaultDoor.h"
#include "VaultRoom.h"
#include "VaultTreasure.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "MI498_UEProject/ScoringSystem/ScoringManager.h"
#include "MI498_UEProject/Weapons/WeaponBase.h"


AMissionController::AMissionController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMissionController::BeginPlay()
{
	Super::BeginPlay();

	ScoringManager = GetGameInstance()->GetSubsystem<UScoringManager>();
	
	/// cache player controller
	PlayerController = GetWorld()->GetFirstPlayerController();

	/// Set beacons invisible
	if (VaultBeacon)
	{
		VaultBeacon->SetActorHiddenInGame(true);
	}

	if (PlayerShipBeacon)
	{
		PlayerShipBeacon->SetActorHiddenInGame(true);
	}

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

	/// The time total time the player has to spend in the vault
	InVaultTime = EnemyWaves.Num() * TimeInBetweenSpawningEnemyWaves;

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

	/// Debug keys
	if (PlayerController->WasInputKeyJustPressed(EKeys::F7))
	{
//		TArray<TSubclassOf<AEnemyBase>> BruteEnemies;
//		BruteEnemies.Add( EnemyWaves[0].Enemies[0]);
//		ParentShip->TrySpawnEnemyUsingEQS(BruteEnemies);
		ExplodeVaultDoor();
	}
	if (PlayerController->WasInputKeyJustPressed(EKeys::F6))
	{
		HandleBombPieceCollected(0);
		HandleBombPieceCollected(1);
		HandleBombPieceCollected(2);
	}
}

void AMissionController::HandleBombPieceCollected(int32 index)
{
	/// Award score for collecting bomb piece
	ScoringManager->AddBombPieceScore();
	BombPiecesCollected++;

	/// Trigger contextual VA based on progress
	if (BombPiecesCollected == 1)
	{
		OnFirstBombPieceCollected(index);
		OnMainMissionObjectiveChange();
	}
	else if (BombPiecesCollected == 2)
	{
		OnSecondBombPieceCollected(index);
		OnMainMissionObjectiveChange();
	}
	else
	{
		OnThirdBombPieceCollected(index);
		OnMainMissionObjectiveChange();

		/// Update the UI icon 5 seconds after (show combined bomb then vault door)
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, index]()
		{
			DelayedCallThirdBombPieceCollected(index);
			OnMainMissionObjectiveChange();
		}, 5.0f, false);

		VaultDoor->EnableInteract();

		/// Enable vault beacon
		if (VaultBeacon)
		{
			VaultBeacon->SetActorHiddenInGame(false);
		}
	}
}

void AMissionController::HandleVaultDoorInteract()
{
	/// Only allow bomb planting during Stage Two
	if (CurrentState == EMissionState::StageTwo)
	{
		OnBombPlanted();

		/// Lock the outside vault door
		OutsideVaultDoor->LockDoor();

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


void AMissionController::ResetBombPlant()
{
	CurrentWave = 0;
	SecondsInVault = 0;
	CloudSpawner->Reset();
	OutsideVaultDoor->LockDoor();
	PlantedBomb->BombAppear();
	OnBombMissionRestart();
	VaultDoor->SetVaultDoorEnabled(true);
	ScoringManager->ResetGlobalScoreMult();
	
	/// Reset loot location and scale
	for (int i = 0; i < VaultTreasure->LootToShrink.Num(); i++)
	{
		VaultTreasure->LootToShrink[i]->SetActorLocation(VaultTreasure->LootOriginalPosition[i]);
		VaultTreasure->LootToShrink[i]->SetActorScale3D(VaultTreasure->LootOriginalScale[i]);
	}
	
	/// reset beacons that are triggered on bomb explode
	if (!PlayerShipBeacon)
	{
		PlayerShipBeacon->SetActorHiddenInGame(true);
	}

	/// Disable vault beacon
	if (!VaultBeacon)
	{
		VaultBeacon->SetActorHiddenInGame(false);
	}

	//Clear Timers
	GetWorldTimerManager().ClearTimer(InVaultTimerHandle);
	GetWorldTimerManager().ClearTimer(MissionTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(EnemyWaveSpawnerTimerHandle);

	/// Delay vault explosion
	GetWorldTimerManager().SetTimer(
		MissionTimerHandle,
		this,
		&AMissionController::ExplodeVaultDoor,
		7,
		false);
	
	/// Unrotate non important espace ships
	for (AShip* Ship : ShipsToRotate)
	{
		if (!Ship) continue;

		FRotator ZeroRotator = FRotator(0,0,0);

		Ship->SetActorRotation(ZeroRotator);
	}
	
	for (AShip* Ship : ShipPathBack)
	{
		if (!Ship) continue;

		FRotator ZeroRotator = FRotator(0,0,0);

		Ship->SetActorRotation(ZeroRotator);
	}
}

void AMissionController::KillAllSpawnedEnemies()
{
	if (ParentShip)
	{
		for (AEnemyBase* enemy : ParentShip->SpawnedEnemies)
		{
			if (!IsValid(enemy)) continue;
			
			if (AAIController* aiController = Cast<AAIController>(enemy->GetController()))
			{
				aiController->StopMovement();
				aiController->UnPossess();
				aiController->Destroy();
			}
		
			if (IsValid(enemy->CurrentWeapon))
			{
				enemy->CurrentWeapon->Destroy();
			}
			
			enemy->Destroy();
		}
	}
	
	ParentShip->SpawnedEnemies.Empty();
}


void AMissionController::ExplodeVaultDoor()
{
	/// Trigger explosion effects and scoring
	PlantedBomb->BombExplode();
	if (bHasCutscenePlayed)
	{
		CutSceneFinish();
	}
	else
	{
		OnBombCutsceneStart();
		PlantedBomb->bHasCutscenePlayed = true;
	}
	VaultDoor->SetVaultDoorEnabled(false);
	/// enable player ship beacon
	if (PlayerShipBeacon)
	{
		PlayerShipBeacon->SetActorHiddenInGame(false);
	}

	/// Disable vault beacon
	if (VaultBeacon)
	{
		VaultBeacon->SetActorHiddenInGame(true);
	}
}

void AMissionController::HandleOnEnterExitPlatform()
{
	/// Level completion condition (Stage Three)
	if (CurrentState == EMissionState::StageThree)
	{
		CurrentState = EMissionState::StageCompleted;
		ScoringManager->AddFinishLevelScore();
		StageThreeFinish(true);
	}
}

void AMissionController::HandleInVaultStatusChange(bool Status)
{
	if (BombPiecesCollected != 3) return;
	if (Status)
	{
		if (!bNearVaultVaLinePlayed)
		{
			bNearVaultVaLinePlayed = true;
			OnNearVault();
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
		
		float Progress = SecondsInVault / InVaultTime;
		if (Progress < 0.2f)
		{
			UpdateScoreImage(5);
		}
		else if (Progress < 0.4f)
		{
			UpdateScoreImage(4);
		}
		else if (Progress < 0.6f)
		{
			UpdateScoreImage(3);
		}
		else if (Progress < 0.8f)
		{
			UpdateScoreImage(2);
		}
		else
		{
			UpdateScoreImage(1);
		}

		/// Make loot slowly dispear
		for (TObjectPtr<AActor> loot : VaultTreasure->LootToShrink)
		{
			// Get actor height 
			float Height = loot->GetSimpleCollisionHalfHeight() * 2.f;

			// Move down by 1 / InVaultTime fraction of height
			FVector Location = loot->GetActorLocation();
			Location.Z -= Height / InVaultTime;

			loot->SetActorLocation(Location);
		}
	}
}

void AMissionController::EndSpawningEnemies()
{
	/// Stop vault timer when all waves are clear
	GetWorldTimerManager().ClearTimer(InVaultTimerHandle);
	
	UpdateScoreImage(0);
	
	OnMainMissionObjectiveChange();

	/// Unlock the outside vault door
	OutsideVaultDoor->UnlockDoor();

	GetWorld()->GetTimerManager().ClearTimer(EnemyWaveSpawnerTimerHandle);

	/// Start mission time by StageThreeAdditionalTime seconds
	float seconds = StageThreeAdditionalTime;

	/// Start the mission timer to flee
	GetWorldTimerManager().ClearTimer(MissionTimerHandle);
	
	OnLeaveVault();
	
	if (!bOnLeaveVaultVaLinePlayed && CurrentState == EMissionState::StageThree)
	{
		bOnLeaveVaultVaLinePlayed = true;
	}

	FTimerDelegate delegate;
	delegate.BindUObject(this, &AMissionController::StageThreeFinish, false);

	GetWorldTimerManager().SetTimer(
		MissionTimerHandle,
		delegate,
		seconds,
		false);
}

void AMissionController::SpawnEnemies(bool bIsActive)
{
	/// End the enemy wave phase
	if (CurrentWave >= EnemyWaves.Num())
	{
		EndSpawningEnemies();
		return;
	}
	
	ParentShip->TrySpawnEnemyUsingEQS(EnemyWaves[CurrentWave].Enemies, EnemySpawnPoints, bIsActive);
	
	// for (int i = 0; i < EnemyWaves[CurrentWave].Enemies.Num(); i++)
	// {
	// 	if (i >= EnemySpawnPoints.Num()) continue;
	//
	// 	AActor* SpawnPoint = EnemySpawnPoints[i];
	//
	// 	if (!SpawnPoint) continue;
	//
	// 	/// Spawn enemy instance
	// 	ParentShip->SpawnEnemyOnShip(EnemyWaves[CurrentWave].Enemies[i],SpawnPoint->GetTransform());
	// }

	CurrentWave++;
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

		/// Start repeating timer while inside vault
		GetWorldTimerManager().SetTimer(
			InVaultTimerHandle,
			this,
			&AMissionController::SecondInVault,
			TimeInVaultToCollectSingleLoot,
			true);

		/// Spawn initial enemy wave
		if (bHasCutscenePlayed)
		{
			SpawnEnemies(true);
		}

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

void AMissionController::CutSceneFinish()
{
	OnBombExplode();
	ScoringManager->AddOpenVaultScore();
	StageTwoFinish(true);
	
	OnMainMissionObjectiveChange();
	
	if (!bHasCutscenePlayed)
	{
		bHasCutscenePlayed = true;
	}
	
	CloudSpawner->Activate();

	/// Rotate non important espace ships
	for (AShip* Ship : ShipsToRotate)
	{
		if (!Ship) continue;
		
		Ship->DestroyAllEnemiesOnShip();

		FRotator RandomRotation = FRotator(
			FMath::RandRange(-180.f, 180.f), // Pitch
			FMath::RandRange(-180.f, 180.f), // Yaw
			FMath::RandRange(-180.f, 180.f)  // Roll
		);

		Ship->SetActorRotation(RandomRotation);
	}
	
	for (AShip* Ship : ShipPathBack)
	{
		if (!Ship) continue;

		FRotator RandomRotation = FRotator(
			FMath::RandRange(-10.f, 10.f), // Pitch
			FMath::RandRange(-10.f, 10.f), // Yaw
			FMath::RandRange(-10.f, 10.f)  // Roll
		);

		Ship->SetActorRotation(RandomRotation);
	}
	
	for (AEnemyBase* enemy : ParentShip->PendingEnemies)
	{
		if (!enemy) continue;
		// reactivate the enemies 
		enemy->SetEnabledEnemy(true);
	}
	
	ParentShip->PendingEnemies.Empty();
}

float AMissionController::GetRemainingMissionTime() const
{
	if (GetWorldTimerManager().GetTimerRemaining(MissionTimerHandle) < 0)
	{
		return 0;
	}
	return GetWorldTimerManager().GetTimerRemaining(MissionTimerHandle);
}
