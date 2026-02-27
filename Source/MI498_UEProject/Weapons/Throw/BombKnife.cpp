// Fill out your copyright notice in the Description page of Project Settings.


#include "BombKnife.h"

#include "BombProjectile.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Characters/Enemies/SwingingEnemy/SwingingEnemy.h"


// Sets default values
ABombKnife::ABombKnife()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABombKnife::PrimaryAttack(AController* Controller, AActor* Target)
{
	Super::PrimaryAttack(Controller, Target);
	
	ASwingingEnemy* character = Cast<ASwingingEnemy>(Controller->GetPawn());
	if (!character || !Target) return;
	USkeletalMeshComponent* enemyMesh = character->GetMesh();
	FVector startLocation;
	if (enemyMesh && enemyMesh->DoesSocketExist(SocketName))
	{
		startLocation = enemyMesh->GetSocketLocation(SocketName) + FVector(0.f, 0.f, character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 120.f) + (character->GetActorForwardVector() *60.f);
	}
	else
	{
		// fallback when there is no socket exists 
		startLocation = character->GetActorLocation()  + FVector(0.f, 0.f, character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 120.f) + (character->GetActorForwardVector() *60.f);
	}
	
	FVector endLocation = Target->GetActorLocation();
	FVector launchVelocity;
	
	
	UGameplayStatics::FSuggestProjectileVelocityParameters params(this,startLocation,endLocation,LaunchSpeed);
	params.bDrawDebug = character->bDebug;
	params.TraceOption = ESuggestProjVelocityTraceOption::DoNotTrace;

	bool bSuccess = UGameplayStatics::SuggestProjectileVelocity(params, launchVelocity);

	if (!bSuccess)
	{
		UE_LOG(EnemyAILog, Error, TEXT("Could not calculate projectile velocity!!"));
		launchVelocity = (endLocation - startLocation).GetSafeNormal() * LaunchSpeed;
	}

	FActorSpawnParameters spawnParams;
	spawnParams.Owner = character;
	spawnParams.Instigator = character;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABombProjectile* bomb = GetWorld()->SpawnActor<ABombProjectile>(
		BombProjectileClass,
		startLocation,
		launchVelocity.Rotation(),
		spawnParams
	);

	if (bomb)
	{
		bomb->Damage = Damage; // Set weapon damage to the projectile 
		bomb->ExplosionRadius = ExplosionRadius;
		bomb->Collision->IgnoreActorWhenMoving(character, true);
		bomb->Movement->Velocity = launchVelocity;
		bomb->Movement->Activate(); // go fly 
	}
}

void ABombKnife::SecondaryAttack(AController* Controller, AActor* Target)
{
	Super::SecondaryAttack(Controller, Target);
	
	if (!Controller || !Controller->GetPawn()) return;
	
	// TODO: Change that to animation notify state when animation is ready!!
	AActor* ownerPawn = Controller->GetPawn();
    
	FVector startLocation = ownerPawn->GetActorLocation(); // this is the start location from the enemy
	FVector forwardLocation = ownerPawn->GetActorForwardVector(); // forward location from the enemy 
	FVector endLocation = startLocation + (forwardLocation * Range); 
	

	FHitResult hitResult;

	// ECC_GameTraceChannel1 is only for the player
	ETraceTypeQuery traceChannel = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1);
	
	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(),startLocation,endLocation,KnifeAttackRadius,traceChannel,false,
		TArray<AActor*>(), // we don't need to ignore anything since the trace channel is dedicated only to the player
		EDrawDebugTrace::None,
		hitResult,
		true
		);
	
	if (bHit && hitResult.GetActor())
	{
		AActor* hitActor = hitResult.GetActor();
		// apply damage 
		UGameplayStatics::ApplyDamage(hitActor, KnifeAttackDamage, Controller, ownerPawn, UDamageType::StaticClass());
	}
}
