#include "SideMissionController.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/ScoringSystem/ScoringManager.h"

/// Cached singleton instance of the SideMissionController
ASideMissionController* ASideMissionController::CachedInstance = nullptr;

ASideMissionController::ASideMissionController()
{
	PrimaryActorTick.bCanEverTick = true;
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

	/// Update timed kill mission (kills within window)
	TryUpdateSubMission("SideObj_06", 1);
	
	// Create a timer handle 
	FTimerHandle TimerHandle;

	// Schedule removal after 10 seconds
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateUObject(this, &ASideMissionController::TryUpdateSubMission, FName("SideObj_06"), -1),
		10.0f,
		false
	);
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

void ASideMissionController::AddPendingSubMission(FName SubMissionRowName, int32 IncrementValue)
{
	int32& currentIncrement = PendingSubMissionUpdates.FindOrAdd(SubMissionRowName);
	currentIncrement += IncrementValue;
}

void ASideMissionController::UpdatePendingSubMission()
{
	/// So i think this needed because when we update the original map and if one of the pending missions 
	/// it will add it to the list again which i'm pretty sure unreal will cry about it....  
	TMap<FName, int32> tempPendingSubMissions = PendingSubMissionUpdates;
    
	PendingSubMissionUpdates.Empty();
	for (const TTuple<FName, int32>& updateSubMission : tempPendingSubMissions)
	{
		if (updateSubMission.Value != 0)
		{
			TryUpdateSubMission(updateSubMission.Key, updateSubMission.Value);
		}
	}
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
}