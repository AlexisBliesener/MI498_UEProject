// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

#include "PistolProjectile.h"
#include "GameFramework/Character.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"


APistol::APistol()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APistol::PrimaryAttack(AController* Controller, AActor* Target)
{
	Super::PrimaryAttack(Controller, Target);
	if (const AEnemyBase* enemy = Cast<AEnemyBase>(Controller->GetPawn()))
	{
		FActorSpawnParameters params;
		params.Owner = Controller->GetPawn();
		params.Instigator = Controller->GetPawn();
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		
		USkeletalMeshComponent* enemyMesh = enemy->GetMesh();
		FVector spawnLocation;
		if (enemyMesh && enemyMesh->DoesSocketExist(SocketName))
		{
			spawnLocation = enemyMesh->GetSocketLocation(SocketName);
		}
		else
		{
			// fallback when there is no socket exists 
			spawnLocation = enemy->GetActorLocation() + FVector(0.f, 20.f, 60.f);
		}
		FVector aimAtLocation = Target->GetActorLocation() + FVector(0.f, 0.f, 60.f);;
		FRotator lookAtRot = (aimAtLocation - spawnLocation).Rotation();
		APistolProjectile* projectile = GetWorld()->SpawnActor<APistolProjectile>(
			ProjectileClass,
			spawnLocation,
			lookAtRot,
			params
		);
		if (projectile)
		{
			projectile->Damage = Damage;
			if (enemy->RealShip)
			{
				// attach the bullet to the ship so it moves with the ship when it's falling  
				projectile->AttachToActor(enemy->RealShip, FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}
}

