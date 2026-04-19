#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
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
	 */
	UFUNCTION()
	AEnemyBase* SpawnEnemyOnShip(TSubclassOf<AEnemyBase> Enemy, FTransform const& Transform);
	
	/**
	 * Activate the ship when the line's trace hit the trace collision box 
	 * @param bIsAiming is the cannon aiming at the ship right now?
	 * @param LastShipActivated If the last ship is not the same as the current ship, then it will deactivate the old one!
	 */
	UFUNCTION(BlueprintCallable, Category = "Ship|Cannon")
	void SetCannonAiming(bool bIsAiming, AShip* LastShipActivated);

protected:
	virtual void Tick(float DeltaSeconds) override;
	float RockTime = 0.f;

	UPROPERTY(EditAnywhere, Category="Ship Movement")
	float RockSpeed = 0.2f;   // how fast it rocks
	float RockOffset = 0.f;
	
	UPROPERTY(EditAnywhere, Category="Ship Movement")
	float RockAngle = 3.0f;  // max degrees each side
	
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
};