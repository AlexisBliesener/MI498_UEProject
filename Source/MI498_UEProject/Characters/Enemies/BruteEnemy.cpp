// Fill out your copyright notice in the Description page of Project Settings.


#include "MI498_UEProject/Characters/Enemies/BruteEnemy.h"

#include "MI498_UEProject/AI/Components/EnemyMovementComponent.h"

ABruteEnemy::ABruteEnemy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UEnemyMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	AutoSuccessRange = AttackStartDistance;
}

void ABruteEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	/// Sets the movement speed of the enemy for animation
	if (AnimationScript)
	{
		AnimationScript->Speed = GetVelocity().Size();
	}
	
}

void ABruteEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	/// Gets reference to the animation script connected on the blueprint
	AnimationScript = Cast<UEnemyAnimation>(GetMesh()->GetAnimInstance());
}


