#include "SideMissionController.h"

#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/ScoringSystem/ScoringManager.h"
#include "MI498_UEProject/Weapons/WeaponTypes.h"

ASideMissionController* ASideMissionController::CachedInstance = nullptr;

ASideMissionController::ASideMissionController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	KilledInTime.Init(-1, 4);
}
void ASideMissionController::BeginPlay()
{
	Super::BeginPlay();

	CachedInstance = this; 
}

ASideMissionController* ASideMissionController::Get(const UObject* WorldContextObject)
{
	if (IsValid(CachedInstance))
	{
		return CachedInstance;
	}

	if (!WorldContextObject) return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;

	CachedInstance = Cast<ASideMissionController>(
		UGameplayStatics::GetActorOfClass(World, ASideMissionController::StaticClass())
	);

	return CachedInstance;
}

void ASideMissionController::KilledEnemy(bool WasAirBorn, EKillType KillType)
{
	if (WasAirBorn)
	{
		TryUpdateSubMission("SideObj_01", 1);
	}
	
	UE_LOG(LogTemp, Log, TEXT("KillType: %s"),
	*UEnum::GetValueAsString(KillType));
	
	if (KillType == EKillType::Sword)
	{
		UE_LOG(LogTemp, Log, TEXT("hit sword"));
		TryUpdateSubMission("SideObj_05", 1);
	}
	
	if (LastKillType != KillType)
	{
		TryUpdateSubMission("SideObj_02", 1);
	}
	else
	{
		ResetSubMission("SideObj_02");
	}
	
	LastKillType = KillType;
	
	if (KillType == EKillType::Barrel)
	{
		TryUpdateSubMission("SideObj_04", 1);
	}
	
	KilledInTime[KilledInTimeIndex] = GetWorld()->GetTimeSeconds();
	TryUpdateSubMission("SideObj_06", 1);
	
	KilledInTimeIndex++;
	KilledInTimeIndex %= 4;
}

void ASideMissionController::InAir()
{
	if (bInAir) return;
	
	bInAir = true;
	TimeInAirStarted = GetWorld()->GetTimeSeconds();
}

void ASideMissionController::HitGround()
{
	bInAir = false;
	ResetSubMission("SideObj_03");
}

void ASideMissionController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (bInAir && GetWorld()->GetTimeSeconds() > TimeInAirStarted + 1)
	{
		TryUpdateSubMission("SideObj_03", 1);
		
		TimeInAirStarted = GetWorld()->GetTimeSeconds();
	}
	
	for (int i = 0; i < 4; i++)
	{
		if (KilledInTime[i] < 0) continue;
		
		if (KilledInTime[i] + 10 < GetWorld()->GetTimeSeconds())
		{
			KilledInTime[i] = -1;
			TryUpdateSubMission("SideObj_06", -1);
		}
	}
}

