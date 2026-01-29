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
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float Damage = 15.f;
	/**
	 * Called when the projectile overlaps with another object to apply damage and destroy itself.
	 * @param OverlappedComp The component that triggered the overlap.
	 * @param OtherActor The actor that the projectile overlapped with.
	 * @param OtherComp The specific component of the other actor involved in the overlap.
	 * @param OtherBodyIndex The index of the body part involved in the overlap.
	 * @param bFromSweep Indicates if this overlap occurred from a sweeping movement.
	 * @param SweepResult Contains details about the overlap event, such as impact location.
	 */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
};
