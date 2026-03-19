// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "BombProjectile.generated.h"

UCLASS()
class MI498_UEPROJECT_API ABombProjectile : public AActor
{
	GENERATED_BODY()
public:
	ABombProjectile();
	/// Damage amount for the bomb, this is going to be set by the weapon!
	float Damage = 8.f;
	/// Collision component for the projectile 
	UPROPERTY(VisibleAnywhere)
	USphereComponent* Collision;
	/// Movement component for the projectile 
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* Movement;
	/// The explosion radius of the bomb, any object/characters on this radius will get damage! 
	float ExplosionRadius = 300.f;
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
		AActor* DamageCauser) override;
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
			   const FHitResult& Hit);

protected:
	virtual void BeginPlay() override;
	
	/// Event for when the bomb is thrown
	UFUNCTION(BlueprintImplementableEvent)
	void OnThrown();
	
	/// Event for when the bomb explodes
	UFUNCTION(BlueprintImplementableEvent)
	void OnExplode();

};
