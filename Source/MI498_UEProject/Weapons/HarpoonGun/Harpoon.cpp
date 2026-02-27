#include "Harpoon.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CableComponent.h"
#include "HarpoonGun.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "MI498_UEProject/Interactables/ExplodingBarrel.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"

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

void AHarpoon::ReturnToPlayer()
{
	bReturnToPlayer = true;
	CurrentReloadingTimeStarted = GetWorld()->GetTimeSeconds();
}

void AHarpoon::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                     FVector NormalImpulse, const FHitResult& Hit)
{
	/// Ignore further hits if already stuck, invalid actor, or hitting the owning player
	if (bStuck || !OtherActor || OtherActor->GetUniqueID() == GetOwner()->GetOwner()->GetUniqueID())
	{
		return;
	}

	/// Blueprint event for SFX
	OnAttach();
	
	/// Set player height variables
	AttachedPlayerHeight = PlayerCharacter->GetActorLocation().Z;
	PrevPlayerHeight = FLT_MAX;
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
	
	/// If hit an exploding barrel
	if (OtherActor)
	{
		if (AExplodingBarrel* barrel = Cast<AExplodingBarrel>(OtherActor))
		{
			barrel->Explode();
			HarpoonGun->Reload();
			return;
		}
	}

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
			this,
			nullptr
		);
	}
}

void AHarpoon::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner()->GetOwner());
	PlayerCharacterMovementComponent = PlayerCharacter->GetCharacterMovement();
}

void AHarpoon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	PrevPlayerHeight = CurrentPlayerHeight;
	CurrentPlayerHeight = PlayerCharacter->GetActorLocation().Z;

	/// Vector from player to harpoon
	FVector toHarpoon = GetActorLocation() - PlayerCharacter->GetActorLocation();
	FVector toHarpoonNormal = toHarpoon.GetSafeNormal();

	if (bReturnToPlayer)
	{
		HandleReturnToPlayer(toHarpoon, toHarpoonNormal, DeltaTime);
	}
	else if (HarpoonGun->IsSwingMode())
	{
		HandleSwing(toHarpoon, toHarpoonNormal, DeltaTime);
	}
	else
	{
		HandleZip(toHarpoon, toHarpoonNormal, DeltaTime);
	}
}

void AHarpoon::HandleReturnToPlayer(const FVector& ToHarpoon, const FVector& ToHarpoonNormal, float DeltaTime)
{
	// Update states
	bFirstSwing = true;
	bReelingPlayerInLastFrame = false;

	// Stop any projectile physics so manual movement takes over
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->Deactivate();

	// Move the harpoon back toward the player at a constant return speed
	SetActorLocation(GetActorLocation() - ToHarpoonNormal * ReturnSpeed * DeltaTime);

	// If the harpoon is close enough to the player destroy this instance
	if (ToHarpoon.Size() < 100.f)
	{
		// Blueprint event for SFX when locking back in
		OnLockIntoGun();
		
		HarpoonGun->DestroyCurrentHarpoon();
	}
}

void AHarpoon::HandleSwing(const FVector& ToHarpoon, const FVector& ToHarpoonNormal, float DeltaTime)
{
	// Update states
	bReelingPlayerInLastFrame = false;

	// If the harpoon is stuck in the world (but not attached to an enemy)
	if (bStuck && !bStuckToEnemy)
	{
		// If player is grounded and rope is stretched beyond its cable length,
		// apply a pulling force toward the anchor to enforce rope constraint
		if (PlayerCharacterMovementComponent->IsMovingOnGround() && ToHarpoon.Size() > CableLength)
		{
			PlayerCharacter->LaunchCharacter(ToHarpoonNormal * PullStrength * DeltaTime,false, false  );
		}
		else
		{
			// Get the player's current velocity
			FVector velocity = PlayerCharacterMovementComponent->Velocity;
				
			// Calculate the velocity component along the rope direction 
			FVector radialVel = FVector::DotProduct(velocity, ToHarpoonNormal) * ToHarpoonNormal;

			// Remove radial component to keep only perpendicular motion 
			FVector tangentialVel = velocity - radialVel;
				
			// On the first swing frame, boost tangential speed
			if (bFirstSwing)
			{
				tangentialVel *= ExtraFirstSwingForce / tangentialVel.Size();
				bFirstSwing = false;
			}
				
			// Apply tangential velocity to maintain swinging motion
			if (!tangentialVel.IsNearlyZero())
			{
				PlayerCharacter->LaunchCharacter(tangentialVel,true,  true );
			}
		}
	}
	// If the harpoon is flying and hasn't attached to anything
	else if (!bStuck && !bStuckToEnemy)
	{
		/// Reload the harpoon if it exceeds its maximum range without hitting anything
		if (ToHarpoon.Size() > Range)
		{
			HarpoonGun->Reload();
		}
	}
}

void AHarpoon::HandleZip(const FVector& ToHarpoon, const FVector& ToHarpoonNormal, float DeltaTime)
{
	/// Update states
	bFirstSwing = true;
	AttachedPlayerHeight = PlayerCharacter->GetActorLocation().Z;
	
	// Harpoon is stuck and attached to an enemy
	if (bStuck && bStuckToEnemy)
	{
		// We are not pulling the player this frame
		bReelingPlayerInLastFrame = false;

		// If enemy is far enough and we are allowed to pull them
		if (ToHarpoon.Size() > 100.f && bPullInEnemy)
		{
			// Prevent collision between harpoon and enemy while pulling
			HarpoonedEnemy->GetCapsuleComponent()->IgnoreActorWhenMoving(this, true);

			// Try pulling enemy directly toward player (full 3D direction)
			if (!HarpoonedEnemy->SetActorLocation(HarpoonedEnemy->GetActorLocation() - ToHarpoonNormal * EnemyPullStrength * DeltaTime,true)) 
			{
				// If blocked , fallback to horizontal-only pull
				HarpoonedEnemy->SetActorLocation(HarpoonedEnemy->GetActorLocation() -FVector(ToHarpoon.X, ToHarpoon.Y, 0.f).GetSafeNormal() *EnemyPullStrength * DeltaTime,true);
			}
		}
		// If enemy is still far but not yet pulling, enable pull mode
		else if (ToHarpoon.Size() > 200.f)
		{
			bPullInEnemy = true;
		}
		// stop pulling enemy
		else
		{
			bPullInEnemy = false;
		}
	}
	// Harpoon stuck in world 
	else if (bStuck)
	{
		// Trigger pull event once when starting to reel player in
		if (!bReelingPlayerInLastFrame)
		{
			OnPullPlayer();
			bReelingPlayerInLastFrame = true;
		}

		// Launch player toward harpoon location
		PlayerCharacter->LaunchCharacter(ToHarpoonNormal * ZipPullStrength * DeltaTime,true, true  );

		// Update cable length as player moves
		CableLength = FVector::Distance(GetActorLocation(),GetOwner()->GetOwner()->GetActorLocation());
	}
	// Harpoon not stuck to anything
	else
	{
		bReelingPlayerInLastFrame = false;

		/// Reload the harpoon if it exceeds its maximum range without hitting anything
		if (ToHarpoon.Size() > Range)
		{
			HarpoonGun->Reload();
		}
	}
}

