// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

#include "PistolProjectile.h"
#include "GameFramework/Character.h"


APistol::APistol()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APistol::PrimaryAttack(AController* Controller, AActor* Target)
{
	if (const ACharacter* character = Cast<ACharacter>(Controller->GetPawn()))
	{
		FVector MuzzleLocation = character->GetMesh()->GetSocketLocation("middle_01_rSocket");
		FRotator LookAtRot = (Target->GetActorLocation() - MuzzleLocation).Rotation();
		FActorSpawnParameters Params;
		Params.Owner = Controller->GetPawn();
		Params.Instigator = Controller->GetPawn();
		GetWorld()->SpawnActor<APistolProjectile>(
			ProjectileClass,
			MuzzleLocation,
			LookAtRot,
			Params
		);
	}
}

