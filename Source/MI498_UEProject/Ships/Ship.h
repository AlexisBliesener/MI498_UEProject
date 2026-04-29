#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "GameFramework/Actor.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "Ship.generated.h"


/**
 * HISM group that convert all similar static mesh to HISM in runtime
 */
USTRUCT(BlueprintType)
struct FHISMGroup
{
	GENERATED_BODY()
	/// The actor class that has the mesh that we want to convert it 
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorClass;
	/// the HISM component after it's converted 
	UPROPERTY()
	class UHierarchicalInstancedStaticMeshComponent* HISMComp = nullptr;
	/// this is a flag used to see if we have the main instance converted or not.. 
	bool bIsCopied = false;
};

/// falls downward when triggered.
UCLASS()
class MI498_UEPROJECT_API AShip : public AActor
{
	GENERATED_BODY()

public:
	AShip();
	
	/// If the player is inside the activation box, the ship and all actors attached to the ship will be enabled (collision)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default|Dev")
	UBoxComponent* ActivationBox;
	/// if the player gets out of the range it will not disable these actors, they will be IGNORED ALWAYS NO MATTER WHAT   
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Dev")
	TArray<AActor*> ActorsIgnored;
	/// Trace Collision Box that will be enabled when the player is out of the ship, and it will be disabled if the player in the ship
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default|Dev")
	UBoxComponent* TraceCollisionBox;
	
	/// Enemies that spawn but they're not active yet
	UPROPERTY()
	TArray<TObjectPtr<AEnemyBase>> PendingEnemies;
	/// All enemies spawned after the vault explosion
	UPROPERTY()
	TArray<TObjectPtr<AEnemyBase>> SpawnedEnemies;
	/// if the cannon is currently aiming at the ship
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsCannonAiming = false;

	/**
	 * Enable/Disable all collision on the ship and turn off all ai systems for the enemies 
	 * @param bIsActive if true, the ship will be active..
	 */
	void SetShipActive(bool bIsActive);

	/**
	 * Add enemy to the ship and set their hidden ship so they can move using the hidden navmesh!
	 * @param Enemy the enemy that to be added
	 * @param Transform transform to spawn the enemy on 
	 * @param bIsActive do you want to activate the enemy when they are spawned?	
	 */
	UFUNCTION()
	AEnemyBase* SpawnEnemyOnShip(TSubclassOf<AEnemyBase> Enemy, FTransform const& Transform, bool bIsActive = true);
	
	/**
	 * Activate the ship when the line's trace hit the trace collision box 
	 * @param bIsAiming is the cannon aiming at the ship right now?
	 * @param LastShipActivated If the last ship is not the same as the current ship, then it will deactivate the old one!
	 */
	UFUNCTION(BlueprintCallable, Category = "Ship|Cannon")
	void SetCannonAiming(bool bIsAiming, AShip* LastShipActivated);
	void DestroyAllEnemiesOnShip();
	
	/**
	 * Start to call the EQS to get a hidden point from the player's view 
	 * @param EnemiesToSpawn enemy type
	 * @param EnemySpawnPoints fallback spawn points if the eqs failed..
	 * @param bIsActive do you want to activate the enemy when they are spawned?	
	 */
	void TrySpawnEnemyUsingEQS(TArray<TSubclassOf<AEnemyBase>> EnemiesToSpawn, TArray<AActor*> EnemySpawnPoints,bool bIsActive);

	/**
	 * This is called after the EQS finished and start to spawn the enemy  
	 * @param Result the final result of the eqs 
	 * @param EnemiesToSpawn what enemies to spawn using these points
	 * @param EnemySpawnPoints fallback list if the eqs failed
	 * @param bIsActive do you want to activate the enemy when they are spawned?	
	 */
	void OnSpawnEQSFinished(TSharedPtr<FEnvQueryResult> Result, TArray<TSubclassOf<AEnemyBase>> EnemiesToSpawn,TArray<AActor*> EnemySpawnPoints, bool bIsActive);

	
	/**
	 * Get the hidden ship reference 
	 * @return the hidden ship
	 */
	AActor* GetHiddenShip() const { return HiddenShip; }
protected:
	/// All actors attached to the ship, they will be added to that list on the start    
	UPROPERTY()
	TArray<AActor*> ActorsOnShip; 
	/// All enemies attached to the ship, they will be added to that list on the start
	UPROPERTY()
	TArray<AEnemyBase*> EnemiesOnShip; 
	/// All actors that we want to convert it from static mesh to hierarchical instanced static mesh
	UPROPERTY(EditAnywhere, Category="Default|Dev")
	TArray<FHISMGroup> ActorsHISMOnShip;
	/// A timer that checks every .5 if the player in that ship 
	FTimerHandle PlayerCheckTimer;
	/// local is player inside the ship?
	bool bIsPlayerInside = false;
	/// Hidden ship related to the current ship (used for the AI navmesh) 
	UPROPERTY()
	AActor* HiddenShip;

	virtual void BeginPlay() override;

	/**
	 * it makes a copy ship far under the map and 
	 * moves the nav mesh on the ship to the copy one so the ai can use it 
	 */
	void DuplicateShipForNavigation();
	
	/**
	 * This function is called on the timer, it will check if the player inside the ship or not 
	 * if the player is inside the ship, the ship will be activated 
	 * otherwise, they will be disabled 
	 */
	void CheckPlayerBox();
	
private:
	/**
	 * It handles the conversion on ActorsHISMOnShip from static mesh to hierarchical instanced static mesh
	 */
	void ConvertSMToHISM();
	
	/// Spawn EQS reference 
	UPROPERTY(EditDefaultsOnly, Category="Default|Dev")
	UEnvQuery* SpawnEQS;
};