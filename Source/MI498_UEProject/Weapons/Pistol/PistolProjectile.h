// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PistolProjectile.generated.h"

UCLASS()
class MI498_UEPROJECT_API APistolProjectile : public AActor
{
	GENERATED_BODY()

public:
	APistolProjectile();
	UPROPERTY(VisibleAnywhere)
	USphereComponent* Collision;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* Movement;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float Damage = 15.f;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
};
