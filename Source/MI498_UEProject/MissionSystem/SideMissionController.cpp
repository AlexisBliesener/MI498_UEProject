#include "SideMissionController.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/ScoringSystem/ScoringManager.h"

/// Cached singleton instance of the SideMissionController
ASideMissionController* ASideMissionController::CachedInstance = nullptr;

ASideMissionController::ASideMissionController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	/// Initialize array to store timestamps of last 4 kills 
	KilledInTime.Init(-1, 4);
}

void ASideMissionController::BeginPlay()
{
	Super::BeginPlay();

	/// Cache this instance for global access
	CachedInstance = this; 
}

ASideMissionController* ASideMissionController::Get(const UObject* WorldContextObject)
{
	/// Return cached instance if valid
	if (IsValid(CachedInstance))
	{
		return CachedInstance;
	}

	/// Validate world context
	if (!WorldContextObject) return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;

	/// Find and cache the controller in the world
	CachedInstance = Cast<ASideMissionController>(
		UGameplayStatics::GetActorOfClass(World, ASideMissionController::StaticClass())
	);

	return CachedInstance;
}

/// Handles logic when an enemy is killed
void ASideMissionController::KilledEnemy(EKillType KillType)
{
	/// Check if player is airborne for aerial kill mission
	if (bInAir)
	{
		TryUpdateSubMission("SideObj_01", 1);
	}
	
	/// Sword kill mission
	if (KillType == EKillType::Sword)
	{
		TryUpdateSubMission("SideObj_05", 1);
	}
	
	/// Alternate kill type combo mission
	if (LastKillType != KillType)
	{
		TryUpdateSubMission("SideObj_02", 1);
	}
	else
	{
		/// Reset combo if same kill type is repeated
		ResetSubMission("SideObj_02");
	}
	
	/// Store last kill type for next comparison
	LastKillType = KillType;
	
	/// Barrel/environmental kill mission
	if (KillType == EKillType::Barrel || KillType == EKillType::KillFloor)
	{
		TryUpdateSubMission("SideObj_04", 1);
	}
	
	/// Store kill timestamp in circular buffer
	KilledInTime[KilledInTimeIndex] = GetWorld()->GetTimeSeconds();

	/// Update timed kill mission (kills within window)
	TryUpdateSubMission("SideObj_06", 1);
	
	/// Advance circular buffer index
	KilledInTimeIndex++;
	KilledInTimeIndex %= 4;
}

/// Called when player enters the air
void ASideMissionController::InAir()
{
	/// Prevent re-triggering if already airborne
	if (bInAir) return;
	
	bInAir = true;

	/// Record start time of airtime
	TimeInAirStarted = GetWorld()->GetTimeSeconds();
}

/// Called when player hits the ground
void ASideMissionController::HitGround()
{
	bInAir = false;

	/// Reset airtime mission progress
	ResetSubMission("SideObj_03");
}

/// Tick function runs every frame
void ASideMissionController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	/// While airborne, increment mission every 1 second
	if (bInAir && GetWorld()->GetTimeSeconds() > TimeInAirStarted + 1)
	{
		TryUpdateSubMission("SideObj_03", 1);
		
		/// Reset timer for next interval
		TimeInAirStarted = GetWorld()->GetTimeSeconds();
	}
	
	/// Check kill timestamps for expiration 
	for (int i = 0; i < 4; i++)
	{
		if (KilledInTime[i] < 0) continue;
		
		/// If kill is too old, remove it and decrement mission progress
		if (KilledInTime[i] + 10 < GetWorld()->GetTimeSeconds())
		{
			KilledInTime[i] = -1;
			TryUpdateSubMission("SideObj_06", -1);
		}
	}
}