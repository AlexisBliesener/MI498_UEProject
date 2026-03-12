// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnemyManagerSubsystem.generated.h"

class AEnemyBase;


/**
 * This system is to manage the ai and enemies references 
 */
UCLASS()
class MI498_UEPROJECT_API UEnemyManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	
	/**
	 * The current enemies that are around the players
	 */
	UPROPERTY()
	TArray<AEnemyBase*> EnemiesAroundPlayer;
};
