// Fill out your copyright notice in the Description page of Project Settings.


#include "MI498_UEProject/Characters/Enemies/BruteEnemy.h"

#include "MI498_UEProject/AI/Components/EnemyMovementComponent.h"

ABruteEnemy::ABruteEnemy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UEnemyMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


