// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MI498_UEProject/Weapons/WeaponBase.h"
#include "Punch.generated.h"

UCLASS()
class MI498_UEPROJECT_API APunch : public AWeaponBase
{
	GENERATED_BODY()

protected:
	virtual void PrimaryAttack(AController* Controller, AActor* Target = nullptr) override;

public:
	// Sets default values for this actor's properties
	APunch();
	// How wide the punch reach is
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float PunchRadius = 70.0f;

	// How far the enemy/player gets pushed
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float KnockbackStrength = 1500.0f;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
