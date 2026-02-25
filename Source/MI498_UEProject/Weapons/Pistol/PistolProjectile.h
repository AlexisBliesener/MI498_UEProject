// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "PistolProjectile.generated.h"

/**
 * Handles movement and interaction behavior for projectile.
 */
UCLASS()
class MI498_UEPROJECT_API APistolProjectile : public AActor
{
	GENERATED_BODY()

public:
	APistolProjectile();
	/// Defines the collision behavior for the projectile using a sphere. 
	UPROPERTY(VisibleAnywhere)
	USphereComponent* Collision;
	///  Manages projectile's speed, direction, and gravity behavior.
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* Movement;
	/// Defines the amount of damage dealt by the projectile.=
	UPROPERTY(NotBlueprintable)
	float Damage = 15.f;
	/**
	 * Called when the projectile hits with another object to apply damage and destroy itself.
	 * @param HitComp The component that triggered the hit.
	 * @param OtherActor The actor that the projectile hit with.
	 * @param OtherComp The specific component of the other actor involved in the hit.
	 * @param NormalImpulse The impulse that applied
	 * @param Hit Information about the hit of a trace
	 */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
protected:
	UPROPERTY(EditDefaultsOnly)
	float DestroyAfterTime = 10.f;
	
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
};
