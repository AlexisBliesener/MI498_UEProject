// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "SwingingEnemy.generated.h"


class ASwingSpline;
class ASwingSplineManager;

/**
 * Swinging enemy class, it holds the spline that the enemy swing on it and other data.
 */
UCLASS()
class MI498_UEPROJECT_API ASwingingEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASwingingEnemy();

	/// current spline for this enemy
	UPROPERTY(BlueprintReadOnly, Category=Output)
	TObjectPtr<ASwingSpline> CurrentSpline;

	/// The distance along the current swing spline
	UPROPERTY(BlueprintReadOnly, Category=Output)
	float SplineDistance = 0.f;
	/// Swing speed for this enemy
	UPROPERTY(EditAnywhere, Category=Default)
	float SwingSpeed = 600.f;
	/// This is used to determine the max radius for the enemy when it requests a new spline 
	UPROPERTY(EditAnywhere, Category=Default)
	float MaxMoveToPointRadius = 250.f;
	/// This is used to determine the max radius for the enemy when it reserves a new spline 
	UPROPERTY(EditAnywhere, Category=Default)
	float MaxReservePointRadius = 2500.f;
	
	/// If the player was in the melee range, then melee attack will trigger! 
	UPROPERTY(EditAnywhere, Category=Default)
	float MeleeRange = 500.f;
	/**
	 * Since this is a swing enemy we want to create a height tolerance 
	 * so the enemy can attack if the player jumps or a little bit 
	 */
	UPROPERTY(EditAnywhere, Category=Default)
	float MeleeZTolerance = 50.f;
	/// Detection radius for x and y points 
	UPROPERTY(EditAnywhere, Category=Default)
	float XYDetectionRadius = 1000.f;
	/**
	 * z distance tolerance in cm for considering the player "under" this enemy
	 * it's used with XYDetectionRadius, it applies only when PlayerZ < EnemyZ
	 */
	UPROPERTY(EditAnywhere, Category=Default)
	float UnderEnemyTolerance = 3000.f;
	/// Cached reference to the SwingSplineManager 
	UPROPERTY()
	TObjectPtr<ASwingSplineManager> Manager;
	/**
	 * Damage amount for the enemy secondary attack
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Weapon")
	float DamageMeleeWeapon  = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Weapon")
	float LaunchSpeed = 1200.f;
	/// Second Attack radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Weapon")
	float SecondAttackRadius = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Weapon")
	float ExplosionRadius = 300.f;
	/// This is used in run time to determine if the enemy swinging right now... 
	UPROPERTY()
	bool bIsSwinging = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	/**
	 * Requests and assigns an available swing spline via Manager
	 * @return true if a swing point assigned to this enemy!
	 */
	bool GetASwingPoint();
};
