// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpNavLinkProxy.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"


AJumpNavLinkProxy::AJumpNavLinkProxy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bSmartLinkIsRelevant = true;
}

void AJumpNavLinkProxy::BeginPlay()
{
	Super::BeginPlay();

	OnSmartLinkReached.AddDynamic(this, &AJumpNavLinkProxy::SmartLinkReached);
}

void AJumpNavLinkProxy::SmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint)
{
	if (AEnemyBase* enemy = Cast<AEnemyBase>(MovingActor))
	{
		FVector localGround = enemy->HiddenShip->GetActorTransform().InverseTransformPosition(DestinationPoint);
		FVector realGround = enemy->RealShip->GetActorTransform().TransformPosition(localGround);
		FVector JumpDir = realGround - enemy->GetActorLocation();
        
		JumpDir.Z = 0.0f;
		JumpDir.Normalize();

		float distance = FVector::Dist2D(enemy->GetActorLocation(), realGround);
		FVector2D jumpForce = FVector2D(distance * 1.2f, enemy->JumpForceNavMesh);

		enemy->OnSmartLinkJump(this);

		enemy->LaunchCharacter((JumpDir * jumpForce.X) + FVector::UpVector * jumpForce.Y, true, true);
	}
}
