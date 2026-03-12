// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "SwingingEnemy.generated.h"

class UStaticMeshComponent;
class UCableComponent;

/**
 * Swinging enemy class, it holds the spline that the enemy swing on it and other data.
 */
UCLASS()
class MI498_UEPROJECT_API ASwingingEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASwingingEnemy(const FObjectInitializer& ObjectInitializer);

	/// If the player was in the melee range, then melee attack will trigger! 
	UPROPERTY(EditAnywhere, Category="Default|Combat")
	float MeleeRange = 150.f;

	/**
	 * Since this is a swing enemy we want to create a height tolerance 
	 * so the enemy can attack if the player jumps or a little bit 
	 */
	UPROPERTY(EditAnywhere, Category="Default|Combat")
	float MeleeZTolerance = 200.f;

	/// This is used in run time to determine if the enemy swinging right now... 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsSwinging = false;
	
	/// How long does the enemy wait before go back to swinging?
	UPROPERTY(EditAnywhere, Category = "Default|Combat")
	float WaitBeforeReturnToSwing = 3.f;
	/// Anchor point for the swinging 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> AnchorMesh;
	/// Swing cable reference 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCableComponent> SwingCable;
	/// Push force (kind of the speed of the swinging)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Swinging")
	float SwingThrustForce = 800.f;
	/// If true, it will draw lines to show swinging ranges  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Swinging")
	bool bDrawDebugSwinging = true;
	/// the top point of the swing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Swinging", meta = (MakeEditWidget = true))
	FVector SwingCenterOffset = FVector(0.f, 0.f, 500.f);
	/// How long is the rope
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Swinging")
	float CableLength = 500.f;
	/// Max angle for the swing 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Swinging", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float MaxSwingAngleDegrees = 60.f;
	/// speed for the rope to go up to the ceiling when the it gets back to swinging!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Swinging")
	float RopeShootSpeed = 6000.f;
	/// speed to pull the enemy up when the rope hits
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Swinging")
	float RopeReelSpeed = 1500.f;
	/// if we want the enemy start at the top of the swing curve
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Swinging")
	bool bStartAtMaxAngle = true;
	/// If true, start swing on the right, false? left!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Swinging", meta = (EditCondition = "bStartAtMaxAngle"))
	bool bStartOnRightSide = true;
	/// distance for the enemy to see and run to the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Combat")
	float ChaseRange = 1500.f;
	/// Launch speed multiplier for arc
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Weapon")
	float LaunchSpeedBomb = 1500.f;
	/// Second Attack damage for this weapon 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Weapon")
	float KnifeAttackDamage = 2.f;
	/// Second Attack radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Weapon")
	float KnifeAttackRadius = 200.f;
	/// The explosion radius of the bomb, any object/characters on this radius will get damage! 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Weapon")
	float ExplosionRadius = 300.f;
	/// distance for the enemy to see and run to the player
	virtual void Tick(float DeltaTime) override;

	/**
	 * detach the rope and make the enemy jump to the ground
	 * @param TargetLocation where the enemy will land
	 */
	UFUNCTION(BlueprintCallable, Category = "Swinging|Actions")
	void DetachAndJumpToGround(FVector TargetLocation);

	/**
	 * find a wall or ceiling and connect the rope
	 */
	UFUNCTION(BlueprintCallable, Category = "Swinging|Actions")
	void AttachToSurface();
	

	/**
	 * This is used when the enemy wants to swing again, 
	 * so they go to this point and then SWING
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector GroundPointUnderSwing;

	/**
	 * throw the rope up and start swinging!
	 */
	UFUNCTION(BlueprintCallable)
	void ShootRopeAndSwing();
	
#if WITH_EDITOR
	virtual bool ShouldTickIfViewportsOnly() const override;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/// Tracks how long the enemy has been stuck
	float StuckTimer = 0.f;

	/// How many seconds the enemy must be standing still before we push them
	UPROPERTY(EditAnywhere, Category = "Default|Dev")
	float StuckTimeThreshold = 0.9f; 

	/// How hard to push them to restart the swing
	UPROPERTY(EditAnywhere, Category = "Default|Dev")
	float StuckPush = 800.f;
private:
	/// direction of the  2D wall for swing math
	FVector SwingPlaneNormal;

	/// rope length saved when we attach to the top
	float RecordedCableLength = 0.f;
	/// is the rope flying up right now?
	bool bIsShootingRope = false;
	/// are we pulling the enemy up right now?
	bool bIsReelingIn = false;
	/// saved world position for the top swing point
	FVector CachedWorldPivot;
	/// local swing pivot on the real ship 
	FVector LocalSwingPivot;
	/**
	 * the math for swing movement....
	 * @param DeltaTime time
	 */
	void HandleSwinging(float DeltaTime);
	/**
	 * find the floor under the swing point
	 * @return a point on the ground 
	 */
	FVector GetGroundPointUnderSwing() const;
	/**
	 * Hide the rope 
	 */
	void Drop();
	
#if WITH_EDITOR
	void DrawSwingPathEditor();
	void DrawCombatRangesEditor();
#endif
};