// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/ProgressBar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/AI/Components/EnemyMovementComponent.h"
#include "MI498_UEProject/Interactables/ExplodingBarrel.h"
#include "MI498_UEProject/Weapons/WeaponBase.h"
#include "MI498_UEProject/Weapons/WeaponInterface.h"
#include "MI498_UEProject/Weapons/Blunderbuss/Blunderbuss.h"
#include "MI498_UEProject/Weapons/HarpoonGun/Harpoon.h"
#include "MI498_UEProject/Weapons/Sword/Sword.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif
DEFINE_LOG_CATEGORY(EnemyLog);
AEnemyBase::AEnemyBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UEnemyMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	UAIPerceptionStimuliSourceComponent* StimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimulusSourceComponent"));
	StimuliSourceComponent->ComponentTags.Add(FName("Player"));
	StimuliSourceComponent->RegisterForSense(TSubclassOf<UAISense_Sight>());
	StimuliSourceComponent->RegisterWithPerceptionSystem();

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(RootComponent);
	HealthBarWidget->SetVisibility(false);
}


void AEnemyBase::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = GetActorLocation();
	OutLocation.Z += EyeHeightOffset; 
	OutRotation = GetActorRotation();
}

void AEnemyBase::SetEnabledEnemy(bool bEnabled)
{
	
	SetActorEnableCollision(bEnabled);
    
	if (IsValid(GetCharacterMovement()))
	{
		GetCharacterMovement()->SetComponentTickEnabled(bEnabled);
	}
    
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->SetActorTickEnabled(bEnabled);
	}
    
	AEnemyAIController* aiController = Cast<AEnemyAIController>(GetController());
	if (IsValid(aiController))
	{
		aiController->SetActorTickEnabled(bEnabled);
       
		UStateTreeEnemyComponent* stateTreeComp = aiController->GetStateTreeAIComponent();
		if (IsValid(stateTreeComp))
		{
			if (bEnabled)
			{
				stateTreeComp->StartStateTree(GetStateTree());
			}
			else
			{
				stateTreeComp->StopStateTree();
			}
		}
       
		UAIPerceptionComponent* perceptionComp = aiController->GetPerceptionComponent();
		if (IsValid(perceptionComp))
		{
			perceptionComp->SetActive(bEnabled);
		}
	}
    
	UAIPerceptionStimuliSourceComponent* stimuliComp = FindComponentByClass<UAIPerceptionStimuliSourceComponent>();
	if (IsValid(stimuliComp))
	{
		stimuliComp->SetActive(bEnabled);
	}
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
	if (RealShip)
	{
		LocalInitLocation = RealShip->GetActorTransform().InverseTransformPosition(EnemyInitLocation);
	}
	
    GridSizeEQS = AttackStartDistance - 300.f;
	
	CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
}

float AEnemyBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	OnTakeDamage();
	
	if (CurrentHealth <= 0.f)
	{
		OnDeath();
		
		/// Add to score
		UScoringManager* ScoringManager = GetGameInstance()->GetSubsystem<UScoringManager>();
		EKillType killType = EKillType::None;
		if (Cast<ABlunderbuss>(DamageCauser)) killType = EKillType::Blunderbuss;
		if (Cast<ASword>(DamageCauser)) killType = EKillType::Sword;
		if (Cast<AHarpoon>(DamageCauser)) killType = EKillType::HarpoonGun;
		if (Cast<AExplodingBarrel>(DamageCauser)) killType = EKillType::Barrel;

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
	UpdateHealthUI();

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

void AEnemyBase::Die()
{
	Super::Die();
	
	float randomFloat = UKismetMathLibrary::RandomFloatInRange(0.0f, 100.0f);

	if (randomFloat <= PercentChanceOfHealthDrop && HealthItemClass)
	{
		// spawn the health item at enemy's location
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		spawnParams.Instigator = GetInstigator();
		GetWorld()->SpawnActor<AActor>(HealthItemClass, GetActorTransform(), spawnParams);
	}
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

void AEnemyBase::UpdateHealthUI() const
{
	if (HealthBarWidget && CameraManager)
	{
		if (UUserWidget* widgetObj = HealthBarWidget->GetUserWidgetObject())
		{
			if (UWidget* foundWidget = widgetObj->GetWidgetFromName(HealthBarWidgetName))
			{
				if (UProgressBar* healthProgressBar = Cast<UProgressBar>(foundWidget))
				{
					healthProgressBar->SetPercent(CurrentHealth / MaxHealth);
				}
			}else
			{
				UE_LOG(EnemyLog, Error, TEXT("Enemy named: %s doesn't have a valid healthbar widget!!"), *GetName());
			}
		}
		
		FRotator lookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CameraManager->GetCameraLocation());
		HealthBarWidget->SetWorldRotation(lookRotation);
		HealthBarWidget->SetVisibility(true);
	}
}
