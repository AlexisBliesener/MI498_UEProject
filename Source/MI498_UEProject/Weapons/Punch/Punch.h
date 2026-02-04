// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MI498_UEProject/Weapons/WeaponBase.h"
#include "Punch.generated.h"

/**
 * A punch class that used by the brute enemy 
 */
UCLASS()
class MI498_UEPROJECT_API APunch : public AWeaponBase
{
	GENERATED_BODY()
public:
	/// Sets default values for this actor's properties
	APunch();
	/// How far the player gets pushed
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float KnockbackStrength = 1500.0f;
	/// How wide the punch reach is
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float PunchRadius = 200.0f;
protected:
	virtual void PrimaryAttack(AController* Controller, AActor* Target = nullptr) override;
};
