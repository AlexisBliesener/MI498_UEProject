#include "Harpoon.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CableComponent.h"
#include "HarpoonGun.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"


class AAIController;

AHarpoon::AHarpoon()
{
	PrimaryActorTick.bCanEverTick = true;

	/// Collision sphere used to detect impacts
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(0.1f);
	Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = Collision;
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->OnComponentHit.AddDynamic(this, &AHarpoon::OnHit);

	/// Projectile movement for initial harpoon flight
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	/// Visual cable connecting the harpoon to the player
	CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	CableComponent->SetupAttachment(RootComponent);
}

void AHarpoon::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                     FVector NormalImpulse, const FHitResult& Hit)
{
	/// Ignore further hits if already stuck, invalid actor, or hitting the owning player
	if (bStuck || !OtherActor || OtherActor->GetUniqueID() == GetOwner()->GetOwner()->GetUniqueID())
	{
		return;
	}

	OnAttach();

	AttachedPlayerHeight = PlayerCharacter->GetActorLocation().Z;
	PrevPlayerHeight = INFINITY;
	CurrentPlayerHeight = PlayerCharacter->GetActorLocation().Z;

	/// Stop projectile movement when the harpoon sticks
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->Deactivate();

	/// Attach the cable end to the owning player's root component
	CableComponent->SetAttachEndToComponent(GetOwner()->GetOwner()->GetRootComponent());

	/// Store the initial rope length when the harpoon hits
	CableLength = FVector::Distance(Hit.ImpactPoint, GetOwner()->GetOwner()->GetActorLocation());

	/// Snap harpoon to the impact point and mark as stuck
	SetActorLocation(Hit.ImpactPoint);
	bStuck = true;

	if (OtherActor)
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AttachToComponent(
			OtherActor->GetRootComponent(),
			FAttachmentTransformRules::KeepWorldTransform
		);
	}

	/// Check if HitResult hit an enemy and apply damage
	if (OtherActor && OtherActor->GetRootComponent()->GetCollisionObjectType() == ECC_Pawn)
	{
		bStuckToEnemy = true;
		HarpoonedEnemy = Cast<AEnemyBase>(OtherActor);
		HarpoonedEnemy->GetCharacterMovement()->DisableMovement();

		UGameplayStatics::ApplyDamage(
			OtherActor,
			HarpoonGun->Damage,
			PlayerCharacter->GetController(),
			GetOwner(),
			nullptr
		);
	}
}

void AHarpoon::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner()->GetOwner());
}

void AHarpoon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	PrevPlayerHeight = CurrentPlayerHeight;
	CurrentPlayerHeight = PlayerCharacter->GetActorLocation().Z;

	/// Vector from player to harpoon
	FVector toHarpoon = GetActorLocation() - PlayerCharacter->GetActorLocation();

	if (bReturnToPlayer)
	{
		bFirstSwing = true;
		bReelingPlayerInLastFrame = false;

		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();

		SetActorLocation(GetActorLocation() - toHarpoon.GetSafeNormal() * ReturnSpeed * DeltaTime);

		if (toHarpoon.Size() < 100.f)
		{
			OnLockIntoGun();
			HarpoonGun->DestroyCurrentHarpoon();
		}
	}
	else if (HarpoonGun->IsSwingMode())
	{
		bReelingPlayerInLastFrame = false;

		if (bStuck && bStuckToEnemy)
		{
		}
		else if (bStuck)
		{
			if (PlayerCharacter->GetCharacterMovement()->IsMovingOnGround() && toHarpoon.Size() > CableLength)
			{
				PlayerCharacter->LaunchCharacter(toHarpoon.GetSafeNormal() * PullStrength * DeltaTime, false,false);
			}
			else
			{
				FVector ropeDir = toHarpoon.GetSafeNormal();

				UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement();
				FVector velocity = MoveComp->Velocity;
				
				FVector radialVel = FVector::DotProduct(velocity, ropeDir) * ropeDir;
				FVector tangentialVel = velocity - radialVel;
				
				if (bFirstSwing)
				{
					tangentialVel *= 1000/tangentialVel.Size();
					bFirstSwing = false;
				}
				
				if (!tangentialVel.IsNearlyZero())
				{
					PlayerCharacter->LaunchCharacter(tangentialVel, true, true);
				}
			}
		}
		else
		{
			/// Reload the harpoon if it exceeds its maximum range without hitting
			if (toHarpoon.Size() > Range)
			{
				HarpoonGun->Reload();
			}
		}
	}
	else
	{
		bFirstSwing = true;
		AttachedPlayerHeight = PlayerCharacter->GetActorLocation().Z;
		if (bStuck && bStuckToEnemy)
		{
			bReelingPlayerInLastFrame = false;

			if (toHarpoon.Size() > 100.f && bPullInEnemy)
			{
				HarpoonedEnemy->GetCapsuleComponent()->IgnoreActorWhenMoving(this, true);
				if (!HarpoonedEnemy->SetActorLocation(HarpoonedEnemy->GetActorLocation()- toHarpoon.GetSafeNormal() * EnemyPullStrength * DeltaTime,true))
				{
					HarpoonedEnemy->SetActorLocation(HarpoonedEnemy->GetActorLocation()- FVector(toHarpoon.X, toHarpoon.Y, 0.f).GetSafeNormal() * EnemyPullStrength * DeltaTime,true);
				}
			}
			else if (toHarpoon.Size() > 200.f)
			{
				bPullInEnemy = true;
			}
			else
			{
				bPullInEnemy = false;
			}
		}
		else if (bStuck)
		{
			if (!bReelingPlayerInLastFrame)
			{
				OnPullPlayer();
				bReelingPlayerInLastFrame = true;
			}

			PlayerCharacter->LaunchCharacter(toHarpoon.GetSafeNormal() * ZipPullStrength * DeltaTime, true, true);
			CableLength = FVector::Distance(GetActorLocation(), GetOwner()->GetOwner()->GetActorLocation());
		}
		else
		{
			bReelingPlayerInLastFrame = false;

			/// Reload the harpoon if it exceeds its maximum range without hitting
			if (toHarpoon.Size() > Range)
			{
				HarpoonGun->Reload();
			}
		}
	}
}
