#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MI498_UEProject/ScoringSystem/ScoringManager.h"
#include "SideMissionController.generated.h"

/// Forward declaration of kill type enum
enum class EKillType : uint8;

UCLASS()
class MI498_UEPROJECT_API ASideMissionController : public AActor
{
	GENERATED_BODY()
	
public:
	ASideMissionController();

	/// Global accessor for the SideMissionController 
	static ASideMissionController* Get(const UObject* WorldContextObject);

	/// Attempts to update a sub-mission by a given increment
	/// Implemented in Blueprint
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TryUpdateSubMission(FName SubMissionRowName, int32 IncrementValue);
	
	/// Resets progress for a given sub-mission
	/// Implemented in Blueprint
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ResetSubMission(FName SubMissionRowName);

	/// Called when an enemy is killed, updates relevant mission logic
	void KilledEnemy(EKillType KillType);
	
	/// Called when the player enters the air
	void InAir();
	
	/// Called when the player lands on the ground
	void HitGround();
	
	/**
	 * Add a submission increment to a list 
	 * you can call UpdatePendingSubMission() Later to call TryUpdateSubMission with the pending values 
	 * @param SubMissionRowName submission row name obviously 
	 * @param IncrementValue how much to increment the row
	 */
	UFUNCTION(BlueprintCallable)
	void AddPendingSubMission(FName SubMissionRowName, int32 IncrementValue);

	/**
	 * Update the pending increment and remove them from the list!
	 */
	UFUNCTION(BlueprintCallable)
	void UpdatePendingSubMission();
protected:

	/// Per-frame update function
	virtual void Tick(float DeltaSeconds) override;

	/// Called when the game starts
	virtual void BeginPlay() override;

private:
	
	/// Whether the player is currently airborne
	bool bInAir = false;

	/// Timestamp of when the player started being in air
	float TimeInAirStarted = 0;
	
	/// Cached singleton instance of this controller
	static ASideMissionController* CachedInstance;
	
	/// Stores the previous kill type for combo/alternation tracking
	EKillType LastKillType = EKillType::None;
	/// Stores the pending updated values!
	TMap<FName, int32> PendingSubMissionUpdates;
};