#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Harpoon.generated.h"

class UCharacterMovementComponent;
class AEnemyBase;
class AHarpoonGun;
class APlayerCharacter;
class UProjectileMovementComponent;
class USphereComponent;
class UCableComponent;

/// Actor representing a fired harpoon projectile
/// Handles flight, collision, cable attachment, and rope constraint logic
UCLASS()
class MI498_UEPROJECT_API AHarpoon : public AActor
{
public:
	AHarpoon();

	/// Sets the maximum range the harpoon can travel before despawning
	void SetRange(const int HarpoonGunRange) { Range = HarpoonGunRange; }

	/// Stores a reference back to the owning harpoon gun
	void SetHarpoonGun(AHarpoonGun* HarpoonGunPtr) { HarpoonGun = HarpoonGunPtr; }

	/// Forces the harpoon to begin returning to the player
	void ReturnToPlayer();

	/// Returns whether the harpoon is currently embedded in something
	bool IsStuck() const { return bStuck; }

	/// Return true if the harpoon is traveling back to the player
	UFUNCTION(BlueprintCallable)
	bool GetReturningToPlayer() const { return bReturnToPlayer; }

	/// Get the time that the harpoon has been reloading for
	UFUNCTION(BlueprintCallable)
	float GetCurrentReloadingTime() const { return GetWorld()->GetTimeSeconds() - CurrentReloadingTimeStarted; }

	/// Projectile movement component controlling harpoon flight
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	/// Cable component used to visually connect the harpoon to the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCableComponent> CableComponent;

	/// Collision sphere used to detect impacts
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Collision;

	/// The strength at which the rope will pull against the player
	UPROPERTY(EditDefaultsOnly)
	int PullStrength = 10000;

	/// The strength at which the harpoon will pull in the player during zip// 
	UPROPERTY(EditDefaultsOnly)
	int ZipPullStrength = 85000;

	/// The strength at which the harpoon will pull in enemies
	UPROPERTY(EditDefaultsOnly)
	int EnemyPullStrength = 1000;

	/// The speed at which the harpoon will travel
	UPROPERTY(EditDefaultsOnly)
	float Speed = 6000;

	/// The speed at which the harpoon will return when reloading
	UPROPERTY(EditDefaultsOnly)
	float ReturnSpeed = 5000;
	
	/// The extra force added in proportion to velocity to the first swing 
	UPROPERTY(EditDefaultsOnly)
	float ExtraFirstSwingForce = 1000;

protected:
	/// Called when the harpoon collides with another actor
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/// A Blueprintable function that will be called when harpoon attaches to something
	UFUNCTION(BlueprintImplementableEvent)
	void OnAttach();

	/// A Blueprintable function that will be called when harpoon pulls player in
	UFUNCTION(BlueprintImplementableEvent)
	void OnPullPlayer();

	/// A Blueprintable function that will be called when harpoon pulls player in
	UFUNCTION(BlueprintImplementableEvent)
	void OnLockIntoGun();

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	/// Handles movement when returning back to the player
	void HandleReturnToPlayer(const FVector& ToHarpoon, const FVector& ToHarpoonNormal, float DeltaTime);
	
	/// Handles rope constraint and swinging logic
	void HandleSwing(const FVector& ToHarpoon, const FVector& ToHarpoonNormal, float DeltaTime);
	
	/// Handles zip-to-point and enemy-pull behavior
	void HandleZip(const FVector& ToHarpoon, const FVector& ToHarpoonNormal, float DeltaTime);

	/// Cached reference to the owning player character
	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter;

	/// Reference to the harpoon gun that fired this projectile
	UPROPERTY()
	TObjectPtr<AHarpoonGun> HarpoonGun;

	/// Whether the harpoon has successfully stuck to a surface
	bool bStuck = false;

	/// Maximum distance the harpoon can travel before despawning
	int Range;

	/// Length of the cable at the moment the harpoon becomes stuck
	float CableLength;

	/// Whether the harpoon is currently returning to the player
	bool bReturnToPlayer = false;

	/// True if the harpoon is attached specifically to an enemy actor
	bool bStuckToEnemy = false;

	/// Enemy currently attached to the harpoon (if any)
	UPROPERTY()
	TObjectPtr<AEnemyBase> HarpoonedEnemy = nullptr;

	/// True if the harpoon should pull in the enemy
	bool bPullInEnemy = true;

	/// True if last frame the harpoon was reeling the player in
	bool bReelingPlayerInLastFrame = false;

	/// The time the harpoon started to reload
	float CurrentReloadingTimeStarted = 0;

	/// Player height at the moment of attachment.
	float AttachedPlayerHeight;

	/// Player height last frame.
	float PrevPlayerHeight = 0;

	/// Player height this frame.
	float CurrentPlayerHeight = 0;

	/// True only for the first swing frame to initialize velocity.
	bool bFirstSwing = true;
	
	/// The players character movement component
	UPROPERTY()
	UCharacterMovementComponent* PlayerCharacterMovementComponent = nullptr;

	GENERATED_BODY()
};
