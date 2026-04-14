// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/WidgetComponent.h"
#include "MI498_UEProject/Characters/CharacterBase.h"
#include "MI498_UEProject/ScoringSystem/ScoringManager.h"
#include "Camera/PlayerCameraManager.h"
#include "MI498_UEProject/AI/Enums/StateTreeEnemyEvents.h"
#include "MI498_UEProject/Weapons/WeaponInterface.h"
#include "EnemyBase.generated.h"

class UStateTree;
class AWeaponBase;
class AJumpNavLinkProxy;
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
	/** The jump force that added when the enemy jump using smart link proxy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Combat")
	float JumpForceNavMesh = 250.f;	
	/**
	 * Damage amount for the enemy primary attack
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Weapon")
	float DamagePrimaryWeapon  = 3.f;
	/**
	 * the percent chance of health drop
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Drops", meta=(ClampMin="0.0", ClampMax="100.0", UIMin="0.0", UIMax="100.0"))
	float PercentChanceOfHealthDrop  = 50.f;
	/// To run eqs it should be attackstartdistance - 100
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Default|Dev")
	float GridSizeEQS = 100.f;
	/**
	 * If not an InvalidRange (which is the default), we will always be able to see the target that has already been seen if they are within this range of their last seen location.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Dev")
	float AutoSuccessRange = -1.0f;
	/**
	 * The health item class to spawn 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Dev")
	TSubclassOf<AActor> HealthItemClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Dev")
	bool bIsAttacking = false;
	
	/**
	 * The current tags that the enemy holds 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Dev")
	FGameplayTagContainer CurrentTags;
	/// This filter is used to hide the jump point (when the state of the enemy is attacking or searching)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default|Dev")
	TSubclassOf<UNavigationQueryFilter> NoAttackJumpFilter;
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
	/// Current state of the enemy
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	StateTreeEnemyEvents CurrentState = StateTreeEnemyEvents::Idle;
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
	virtual void Die() override;
	
	/// Event for when the enemy takes damage
	UFUNCTION(BlueprintImplementableEvent)
	void OnTakeDamage();
	
	/// Event for when the enemy dies
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath();
	
	/// Event for when the jump starts
	UFUNCTION(BlueprintImplementableEvent)
	void OnJumpStart();
	
	/// Event for when the jump ends 
	UFUNCTION(BlueprintImplementableEvent)
	void OnJumpEnd();
	
	/// Event for when the enemy does its primary attack
	UFUNCTION(BlueprintImplementableEvent)
	void OnPrimaryAttack(AWeaponBase* Weapon, AActor* Target);
	
	/// Event for when the enemy does its seconodary attack
	UFUNCTION(BlueprintImplementableEvent)
	void OnSecondaryAttack(AWeaponBase* Weapon, AActor* Target);
	
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	/**
	 * Enable/disable the AI system/Collision and character movement for the enemy 
	 * @param bEnabled if true, it will activate the enemy 
	 */
	void SetEnabledEnemy(bool bEnabled);
	
	/**
	 * Stun the enemy they won't be able to do anything.. poor enemy.. 
	 */
	void StunMe();
	/**
	* Release the enemy they will move to attack after that  
	*/
	void StunEnd();
	
	/**
	 * This is called when the enemy reaches a jump nav link point 
	 * @param InNavLink the NavLinkProxy 
	 */
	UFUNCTION()
	void OnSmartLinkJump(AJumpNavLinkProxy* InNavLink);
	
	/**
	 * Can the enemy shoot? 
	 * @return true, if the enemy can shoot 
	 */
	bool GetCanShoot() const {return bCanShoot;}
	
	/**
	 * Is the enemy currently facing the player? the attack won't happen until this becomes true !!
	 * @param Player the target which is the player 
	 * @param Tolerance tolerance 
	 * @return 
	 */
	bool IsFacingPlayer(AActor* Player, float Tolerance) const;
protected:
	
	/// Fires the primary weapon from blueprint
	/// Used so the attack animation calls fire at the correct time
	UFUNCTION(BlueprintCallable)
	void FirePrimaryAttack(AWeaponBase* Weapon, AActor* Target);
	
	/// Fires the secondary weapon from blueprint
	/// Used so the attack animation calls fire at the correct time
	UFUNCTION(BlueprintCallable)
	void FireSecondaryAttack(AWeaponBase* Weapon, AActor* Target);
	
	/// Reference for the health bar widget component 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Health")
	UWidgetComponent* HealthBarWidget;
	/// This should match exactly the widget name! 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Dev")
	FName HealthBarWidgetName = TEXT("EnemyHealthBar");
	/**
	 * The VFX when the enemy gets hit by a blunderbuss
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|VFX|Hit")
	UParticleSystem* HitBlunderbussVFX;
	/**
	 * The VFX when the enemy gets hit by a sword
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|VFX|Hit")
	UParticleSystem* HitSwordVFX;
	/**
	 * The VFX when the enemy gets hit by a harpoongun
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|VFX|Hit")
	UParticleSystem* HitHarpoonGunVFX;
	/**
	 * The VFX when the enemy dies
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|VFX|Death")
	UParticleSystem* DeathVFX;
	/**
	 * How big the hit Blunderbuss VFX should be
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|VFX|Hit")
	FVector HitBlunderbussVFXScale = FVector(0.2f, 0.2f, 0.2f);
	/**
	 * How big the hit sword VFX should be
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|VFX|Hit")
	FVector HitSwordVFXScale = FVector(0.2f, 0.2f, 0.2f);
	/**
	 * How big the hit HarpoonGun VFX should be
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|VFX|Hit")
	FVector HitHarpoonGunVFXScale = FVector(0.2f, 0.2f, 0.2f);

	/**
	 * How big the death VFX should be
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|VFX|Death")
	FVector DeathVFXScale = FVector(0.5f, 0.5f, 0.5f);
	/// Is the enemy currently jumping? 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsJumping = false;
	
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
	virtual void Landed(const FHitResult& Hit) override;
private:
	///  State tree used for AI logic of the player or enemy.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|AI", meta = (AllowPrivateAccess = "true"))
	UStateTree* CurrentStateTree;
	/// Indicates if the enemy can currently shoot to prevents shooting during cooldown.
	bool bCanShoot = true;
	/// Current nav link proxy when the jump happened 
	UPROPERTY()
	TObjectPtr<AJumpNavLinkProxy> CurrentNavLink = nullptr;
	/// Timer used to manage the cooldown period between enemy shots
	FTimerHandle ShootTimer;
	/**
	 * Resets the shooting ability of the enemy.
	 */
	void ResetShoot();
	
	/**
	 * This is called on damaged, it will update the value of the health bar and show it if it was invisible,
	 * and it will face the player   
	 */
	void UpdateHealthUI() const;
};
