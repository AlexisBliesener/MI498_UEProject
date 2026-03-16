// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MI498_UEProject/Animation/EnemyAnimation.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "BruteEnemy.generated.h"

/**
 * 
 */
UCLASS()
class MI498_UEPROJECT_API ABruteEnemy : public AEnemyBase
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	ABruteEnemy(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	
private:
	UEnemyAnimation* BruteAnimation;

};
