// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Weapons/Pistol/PistolProjectile.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
	UAIPerceptionStimuliSourceComponent* StimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimulusSourceComponent"));
	StimuliSourceComponent->ComponentTags.Add(FName("Player"));
	StimuliSourceComponent->RegisterForSense(TSubclassOf<UAISense_Sight>());
	StimuliSourceComponent->RegisterWithPerceptionSystem();
	CurrentHealth = MaxHealth;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
}

float AEnemyBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,class AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageAmount <= 0.f || CurrentHealth <= 0.f)
		return 0.f;

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);

	if (CurrentHealth <= 0.f)
	{
		// DIE i think
		UE_LOG(LogTemp, Error, TEXT("Enemy DIED"));
		
		// Stop AI
		if (AAIController* AI = Cast<AAIController>(GetController()))
		{
			AI->StopMovement();
			AI->UnPossess();
		}

		// Disable collision & movement
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->DisableMovement();

		SetLifeSpan(0.5f); 
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
}

void AEnemyBase::UnPossessed()
{
	Super::UnPossessed();
}

UStateTree* AEnemyBase::GetStateTree() const
{
	return StateTreesss;
}

void AEnemyBase::Attack(AActor* Target)
{

	if (!Target || !ProjectileClass || !bCanShoot)
		return;

	const float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	if (Distance > AttackRange)
		return;
	
	FVector MuzzleLocation = GetMesh()->GetSocketLocation("middle_01_rSocket");
	FRotator LookAtRot = (Target->GetActorLocation() - MuzzleLocation).Rotation();

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;

	GetWorld()->SpawnActor<APistolProjectile>(
		ProjectileClass,
		MuzzleLocation,
		LookAtRot,
		Params
	);

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
