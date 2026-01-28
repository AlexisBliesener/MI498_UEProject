// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MI498_UEProject/Characters/CharacterBase.h"
#include "MI498_UEProject/Weapons/Pistol/PistolProjectile.h"
#include "EnemyBase.generated.h"

/**
 * Base class for enemy characters in the game.
 * Can attack, chase, and take damage.
 */
UCLASS(Blueprintable, meta=(AllowDerivedTypes="true"))
class MI498_UEPROJECT_API AEnemyBase : public ACharacterBase
{
public:
	GENERATED_BODY()
	// Distance within which the enemy can attack a target.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Combat")
	float AttackRange = 1000.f;
	// Time between enemy shots, in seconds.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Combat")
	float ShootCooldown = 1.2f;
	// Speed at which the enemy chases the target.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Movement")
	float ChaseSpeed = 450.f;
	UPROPERTY(EditDefaultsOnly, Category="Enemy|Combat")
	TSubclassOf<APistolProjectile> ProjectileClass;
	/**
	 * Initializes default properties and components for the enemy character
	 */
	AEnemyBase();
	
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	/**
	 * Gets the state tree used by the enemy.
	 * @return The state tree assigned to the enemy.
	 */
	UFUNCTION(BlueprintPure, Category = "Player|AI", meta = (DisplayName = "Get State Tree", ReturnDisplayName = "State Tree"))
	UStateTree* GetStateTree() const;

	/**
	 * Makes the enemy attack the given target.
	 * @param Target The actor to be attacked.
	 */
	UFUNCTION(BlueprintCallable, Category="Enemy|Combat")
	void Attack(AActor* Target);
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
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,AActor* DamageCauser) override;
private:
	//  State tree used for AI logic of the player or enemy.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|AI", meta = (AllowPrivateAccess = "true"))
	UStateTree* StateTreesss;
	// Indicates if the enemy can currently shoot to prevents shooting during cooldown.
	bool bCanShoot = true;
	// Timer used to manage the cooldown period between enemy shots
	FTimerHandle ShootTimer;
	/**
	 * Resets the shooting ability of the enemy.
	 */
	void ResetShoot();
};
