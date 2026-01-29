// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Weapons/WeaponBase.h"
#include "MI498_UEProject/Weapons/WeaponInterface.h"
#include "MI498_UEProject/Weapons/Pistol/PistolProjectile.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
DEFINE_LOG_CATEGORY(EnemyLog);
AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
	UAIPerceptionStimuliSourceComponent* StimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimulusSourceComponent"));
	StimuliSourceComponent->ComponentTags.Add(FName("Player"));
	StimuliSourceComponent->RegisterForSense(TSubclassOf<UAISense_Sight>());
	StimuliSourceComponent->RegisterWithPerceptionSystem();

}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	if (WeaponBlueprint)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;

		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponBlueprint, SpawnParams);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("middle_01_rSocket"));

	}
	
	if (!CurrentWeapon)
	{
		UE_LOG(EnemyLog, Error, TEXT("Enemy named: %s doesn't have a weapon!!"), *GetName());
	}
}

float AEnemyBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (CurrentHealth <= 0.f)
	{
		// DIE i think
		UE_LOG(EnemyLog, Error, TEXT("Enemy DIED"));
		
		// Stop AI
		if (AAIController* AI = Cast<AAIController>(GetController()))
		{
			AI->StopMovement();
			AI->UnPossess();
		}

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->DisableMovement();

		Destroy();
	}

	return DamageAmount;
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Apply the data asset settings for the specific enemy 
	if (AISettings)
	{
		ChaseSpeed = AISettings->ChaseSpeed;
		AttackRange = AISettings->AttackRange;
		ShootCooldown = AISettings->ShootCooldown;

		if (AEnemyAIController* AIController = Cast<AEnemyAIController>(NewController))
		{
			if (AIController->SightConfig)
			{
				AIController->SightConfig->SightRadius = AISettings->SightRadius;
				AIController->SightConfig->LoseSightRadius = AISettings->LoseSightRadius;
				AIController->SightConfig->PeripheralVisionAngleDegrees = AISettings->PeripheralVisionAngle;
			}
			if (AIController->HearingConfig)
			{
				AIController->HearingConfig->HearingRange = AISettings->HearingRange;
			}
		}
	}
}

void AEnemyBase::UnPossessed()
{
	Super::UnPossessed();
}

UStateTree* AEnemyBase::GetStateTree() const
{
	return CurrentStateTree;
}

void AEnemyBase::Attack(AActor* Target)
{

	if (!Target || !bCanShoot)
		return;

	const float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	if (Distance > AttackRange)
	{
		return;
	}
	if (!CurrentWeapon)
	{
		UE_LOG(EnemyLog, Error, TEXT("Enemy named: %s doesn't have a weapon!!"), *GetName());
		return;
	}
	if (IWeaponInterface* Weapon = Cast<IWeaponInterface>(CurrentWeapon))
	{
		Weapon->PrimaryAttack(GetController(), Target);
	}

	// cooldown
	GetWorldTimerManager().SetTimer(
		ShootTimer,
		this,
		&AEnemyBase::ResetShoot,
		ShootCooldown,
		false
	);
	bCanShoot = false;
}

void AEnemyBase::ResetShoot()
{
	bCanShoot = true;
}
