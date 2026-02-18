// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Weapons/WeaponBase.h"
#include "MI498_UEProject/Weapons/WeaponInterface.h"
#include "MI498_UEProject/Weapons/Blunderbuss/Blunderbuss.h"
#include "MI498_UEProject/Weapons/HarpoonGun/Harpoon.h"
#include "MI498_UEProject/Weapons/Sword/Sword.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif
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
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		CurrentWeapon->Damage = DamagePrimaryWeapon;

	}
	
	if (!CurrentWeapon)
	{
		UE_LOG(EnemyLog, Error, TEXT("Enemy named: %s doesn't have a weapon!!"), *GetName());
	}
	
	// initial location of the enemy when spawn
	EnemyInitLocation = GetActorLocation();
}

float AEnemyBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (CurrentHealth <= 0.f)
	{
		// DIE i think
		UE_LOG(EnemyLog, Error, TEXT("Enemy DIED"));
		
		/// Add to score
		UScoringManager* ScoringManager = GetGameInstance()->GetSubsystem<UScoringManager>();
		EKillType killType = EKillType::None;
		if (Cast<ABlunderbuss>(DamageCauser)) killType = EKillType::Blunderbuss;
		if (Cast<ASword>(DamageCauser)) killType = EKillType::Sword;
		if (Cast<AHarpoon>(DamageCauser)) killType = EKillType::HarpoonGun;

		ScoringManager->AddKillEnemyScore(EnemyType, killType);
		
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

bool AEnemyBase::ShouldTickIfViewportsOnly() const
{
	// Only tick in the editor if Debug is enabled 
	return bDebug;
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
#if WITH_EDITOR
	if (bDebug)
	{
		if (GetWorld() && !GetWorld()->IsGameWorld())
		{
			FMatrix circleMatrix = FRotationMatrix::MakeFromX(FVector::UpVector) * FTranslationMatrix(GetActorLocation());
			// Draw Attack Start Distance (YELLOW)
			DrawDebugCircle(
				GetWorld(),
				circleMatrix,
				AttackStartDistance,
				32,             
				FColor::Yellow,
				false,          
				-1.0f,          
				0,               
				2.0f,              
				false             
			);

			// Draw Attack Perform Distance (RED)
			DrawDebugCircle(
				GetWorld(),
				circleMatrix,
				AttackPerformDistance,
				32,
				FColor::Red,
				false,
				-1.0f,
				0,
				2.0f,
				false
			);
		}
	}
#endif
	
	
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
	return CurrentStateTree;
}

void AEnemyBase::Attack(AActor* Target, bool bIsSecondaryAttack)
{

	if (!Target || !bCanShoot)
		return;
	
	if (!CurrentWeapon)
	{
		UE_LOG(EnemyLog, Error, TEXT("Enemy named: %s doesn't have a weapon!!"), *GetName());
		return;
	}
	if (IWeaponInterface* Weapon = Cast<IWeaponInterface>(CurrentWeapon))
	{
		if (bIsSecondaryAttack)
		{
			Weapon->SecondaryAttack(GetController(), Target);
		}else
		{
			Weapon->PrimaryAttack(GetController(), Target);
		}
	}

	// cooldown
	GetWorldTimerManager().SetTimer(
		ShootTimer,
		this,
		&AEnemyBase::ResetShoot,
		AttackCooldown,
		false
	);
	bCanShoot = false;
}


void AEnemyBase::ResetShoot()
{
	bCanShoot = true;
}
