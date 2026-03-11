// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MI498_UEProject/Characters/CharacterBase.h"
#include "MI498_UEProject/ScoringSystem/ScoringManager.h"
#include "EnemyBase.generated.h"

class AWeaponBase;
DECLARE_LOG_CATEGORY_EXTERN(EnemyLog, Log, All);

/**
 * Base class for enemy characters in the game.
 * Can attack, chase, and take damage.
 */
UCLASS(Blueprintable, meta=(AllowDerivedTypes="true"))
class MI498_UEPROJECT_API AEnemyBase : public ACharacterBase
{
public:
	GENERATED_BODY()
	/**
	 * This is the range when enemy start to start the attacking behavior.
	 * For example, if this player is in this distance, that means the enemy sees the player
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Combat")
	float AttackStartDistance = 1000.f;
	/**
	 * This is the range when enemy start to preform the attack (Shooting for example).
	 * Usually is less than the attack start distance.
	 * So the enemy can get closer to the player and then ATTACK!
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Combat")
	float AttackPerformDistance  = 500.f;
	/**
	 * The radius to generate a random patrol point around the spawn point of the enemy.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Combat")
	float PatrolRadius  = 1000.f;
	/// Time between enemy shots, in seconds.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Weapon")
	float AttackCooldown = 1.2f;
	/**
	 * How high the eyes are from the center of the enemy (it used to detect the player) 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Combat")
	float EyeHeightOffset = 50.f;
	/**
	 * How many seconds the enemy waits at the last known player location
	 * after losing sight and before returning to patrol
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Combat")
	float WaitAtLastKnownLocationTime = 3.f;
	/** Maximum sight distance to not see the target that has been already seen. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Combat")
	float LoseSightRadius = AttackStartDistance + 300.f;
	/**
	 * Damage amount for the enemy primary attack
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Weapon")
	float DamagePrimaryWeapon  = 3.f;
	/// To run eqs it should be attackstartdistance - 100
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Default|Dev")
	float GridSizeEQS = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Dev")
	float AutoSuccessRange = -1.0f;
	/// The type of this enemy used for scoring
	UPROPERTY(EditAnywhere)
	EEnemyType EnemyType;
	UPROPERTY()
	FVector AssignedLocation;
	/**
	 * Gets the state tree used by the enemy.
	 * @return The state tree assigned to the enemy.
	 */
	UFUNCTION(BlueprintPure, Category = "Player|AI",
		meta = (DisplayName = "Get State Tree", ReturnDisplayName = "State Tree"))
	UStateTree* GetStateTree() const;
	/// The currently weapon
	UPROPERTY()
	AWeaponBase* CurrentWeapon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Default)
	TSubclassOf<AWeaponBase> WeaponBlueprint;
	/// THIS IS A TEMP DEBUG FOR DEV
	UPROPERTY(EditAnywhere, Category=Default)
	bool bDebug = false;
	/// Initial location of the enemy when spawns
	UPROPERTY(BlueprintReadOnly)
	FVector EnemyInitLocation;
	/// Initial local location of the enemy when spawns
	UPROPERTY(BlueprintReadOnly)
	FVector LocalInitLocation;
	/// A reference to the real ship that the enemy is on it 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> RealShip;
	/// A reference to the fake ship (that has the nav mesh) that the enemy is on it 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> HiddenShip;
	/**
	 * Makes the enemy attack the given target.
	 * @param Target The actor to be attacked.
	 * @param bIsSecondaryAttack is this a secondary attack?
	 */
	UFUNCTION(BlueprintCallable, Category=Default)
	virtual void Attack(AActor* Target, bool bIsSecondaryAttack = false);
	/**
	 * Initializes default properties and components for the enemy character
	 */
	AEnemyBase(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	
	/// Event for when the enemy takes damage
	UFUNCTION(BlueprintImplementableEvent)
	void OnTakeDamage();
	
	/// Event for when the enemy dies
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath();
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
protected:
	virtual void BeginPlay() override;
	/**
	 * Applies damage to the enemy and handles its death if health reaches zero.
	 * @param DamageAmount The amount of damage to be applied.
	 * @param DamageEvent Details about the damage event.
	 * @param EventInstigator The controller responsible for causing the damage.
	 * @param DamageCauser The actor that caused the damage.
	 * @return The actual amount of damage applied to the enemy.
	 */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	                         class AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual bool ShouldTickIfViewportsOnly() const override;
private:
	///  State tree used for AI logic of the player or enemy.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|AI", meta = (AllowPrivateAccess = "true"))
	UStateTree* CurrentStateTree;
	/// Indicates if the enemy can currently shoot to prevents shooting during cooldown.
	bool bCanShoot = true;
	/// Timer used to manage the cooldown period between enemy shots
	FTimerHandle ShootTimer;
	/**
	 * Resets the shooting ability of the enemy.
	 */
	void ResetShoot();
};
