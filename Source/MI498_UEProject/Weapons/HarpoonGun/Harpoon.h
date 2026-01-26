#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Harpoon.generated.h"

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
	
	/// The speed at which the harpoon will travel
	UPROPERTY(EditDefaultsOnly)
	float Speed = 6000;

protected:
	
	/// Called when the harpoon collides with another actor
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	/// Cached reference to the owning player character
	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter;
	
	/// Reference to the harpoon gun that fired this projectile
	UPROPERTY()
	TObjectPtr<AHarpoonGun> HarpoonGun;
	
	/// Whether the harpoon has successfully stuck to a surface
	bool Stuck = false;
	
	/// Maximum distance the harpoon can travel before despawning
	int Range;
	
	/// Length of the cable at the moment the harpoon becomes stuck
	float CableLength;

	GENERATED_BODY()
};
