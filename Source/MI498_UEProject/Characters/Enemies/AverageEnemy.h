// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "AverageEnemy.generated.h"

class UEnemyAnimation;
/**
 * 
 */
UCLASS()
class MI498_UEPROJECT_API AAverageEnemy : public AEnemyBase
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AAverageEnemy(const FObjectInitializer& ObjectInitializer);
	
	/**
	 * How many seconds the enemy waits after shooting before moving again?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Combat")
	float DelayAfterShootBeforeMove = 0.5f;
	
protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	
private:
	/// This enemies animation script
	UPROPERTY()
 	UEnemyAnimation* AnimationScript = nullptr;
};
