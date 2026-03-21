#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MI498_UEProject/ScoringSystem/ScoringManager.h"
#include "MI498_UEProject/Weapons/WeaponTypes.h"
#include "SideMissionController.generated.h"

enum class EKillType : uint8;
enum class EWeaponType : uint8;

UCLASS()
class MI498_UEPROJECT_API ASideMissionController : public AActor
{
	GENERATED_BODY()
	
public:
	ASideMissionController();
	static ASideMissionController* Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TryUpdateSubMission(FName SubMissionRowName, int32 IncrementValue);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ResetSubMission(FName SubMissionRowName);
	void KilledEnemy(bool WasAirBorn, EKillType KillType);
	
	void InAir();
	
	void HitGround();
	
protected:

	virtual void Tick(float DeltaSeconds) override;	
	virtual void BeginPlay() override;
private:
	
	bool bInAir = false;
	float TimeInAirStarted = 0;
	
	TArray<float> KilledInTime;
	
	int KilledInTimeIndex = 0;
	
	static ASideMissionController* CachedInstance;
	
	EKillType LastKillType = EKillType::None;
};
