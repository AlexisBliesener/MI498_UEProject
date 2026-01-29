// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemySettings.generated.h"

/**
 * 
 */
UCLASS()
class MI498_UEPROJECT_API UEnemySettings : public UDataAsset
{
public:
	GENERATED_BODY()
	/// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float ChaseSpeed = 450.f;
	/// Combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float AttackRange = 1000.f;
    /// Attack cool down
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float ShootCooldown = 1.2f;
	/// Sight Radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perception|Sight")
	float SightRadius = 1000.f;
	/// Lose sight radius 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perception|Sight")
	float LoseSightRadius = 1500.f;
	/// Peripheral vision angle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perception|Sight")
	float PeripheralVisionAngle = 35.f;
	/// Hearing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perception|Hearing")
	float HearingRange = 1200.f;
};
