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
	// Release the enemy and reset their state if the enemy was attached to the harpoon
	if (bStuckToEnemy && IsValid(HarpoonedEnemy))
	{
		HarpoonedEnemy->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		HarpoonedEnemy = nullptr;
		bStuckToEnemy = false;
	}
}

void AHarpoon::ChangeSocketAttachment(bool HarpoonGunOut)
{
	/// Delay the socket switch slightly to line up with animation 
	GetWorld()->GetTimerManager().SetTimer(
		SocketSwitchTimer,
		FTimerDelegate::CreateLambda([this, HarpoonGunOut]()
		{
			/// Attach the cable to the harpoon gun socket when the gun is equipped
			if (HarpoonGunOut)
			{
				CableComponent->SetAttachEndToComponent(
					PlayerCharacter->GetMesh(),
					TEXT("HarpoonGunBaseSocket")
				);
			}
			/// Otherwise attach the cable to the thigh socket when the gun is holstered
			else
			{
				CableComponent->SetAttachEndToComponent(
					PlayerCharacter->GetMesh(),
					TEXT("Thigh_LSocket")
				);
			}
		}),
		0.1f,
		false
	);
}

void AHarpoon::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                     FVector NormalImpulse, const FHitResult& Hit)
{
	/// Ignore further hits if already stuck, invalid actor, or hitting the owning player
	if (bStuck || !OtherActor || OtherActor->GetUniqueID() == PlayerCharacter->GetUniqueID())
	{
		return;
	}

	/// Blueprint event for SFX
	OnAttach();

	/// Set player height variables
	AttachedPlayerHeight = PlayerCharacter->GetActorLocation().Z;
	PrevPlayerHeight = 10000000000;
	CurrentPlayerHeight = PlayerCharacter->GetActorLocation().Z;

	/// Stop projectile movement when the harpoon sticks
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->Deactivate();

	/// Store the initial rope length when the harpoon hits
	CableLength = FVector::Distance(Hit.ImpactPoint, PlayerCharacter->GetActorLocation());

	/// Snap harpoon to the impact point and mark as stuck
	SetActorLocation(Hit.ImpactPoint);
	bStuck = true;
	PreviousAnchorLocation = GetActorLocation();

	if (PlayerCharacter)
	{
		if (USkeletalMeshComponent* Mesh = PlayerCharacter->GetMesh())
		{
			if (UPlayerAnimation* Anim = Cast<UPlayerAnimation>(Mesh->GetAnimInstance()))
			{
				Anim->SetHarpoonAttached(true);
			}
		}
	}

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

		/// Attach harpoon to actor it hit
		FTransform WorldTransform = GetActorTransform();
		AttachToComponent(
			OtherComp,
			FAttachmentTransformRules::KeepWorldTransform,
			Hit.BoneName);
		SetActorTransform(WorldTransform);
	}

	/// Check if HitResult hit an enemy and apply damage
	if (OtherActor && OtherActor->GetRootComponent()->GetCollisionObjectType() == ECC_Pawn)
	{
		bStuckToEnemy = true;
		HarpoonedEnemy = Cast<AEnemyBase>(OtherActor);
		HarpoonedEnemy->GetCharacterMovement()->DisableMovement();

		/// Return harpoon if the shot kills the enemy
		if (HarpoonedEnemy->CurrentHealth - HarpoonGun->Damage <= 0)
		{
			ReturnToPlayer();
		}

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

	if (PlayerCharacter && PlayerCharacterMovementComponent)
	{
		// Ignore collisions with player
		Collision->IgnoreActorWhenMoving(PlayerCharacter, true);

		// Attach cable to gun
		CableComponent->bAttachEnd = true;
		CableComponent->SetAttachEndToComponent(PlayerCharacter->GetMesh(), TEXT("HarpoonGunBaseSocket"));

		// Use the camera forward vector
		FVector CameraLocation;
		FRotator CameraRotation;
		PlayerCharacter->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
		FVector FireDirection = CameraRotation.Vector().GetSafeNormal();

		// Add player velocity for momentum
		FVector InitialVelocity = FireDirection * Speed + PlayerCharacterMovementComponent->Velocity;

		// Set projectile movement
		ProjectileMovement->Velocity = InitialVelocity;
		ProjectileMovement->InitialSpeed = InitialVelocity.Size();
		ProjectileMovement->MaxSpeed = InitialVelocity.Size();

		// Make sure gravity is off
		ProjectileMovement->ProjectileGravityScale = 0.f;
	}
}

void AHarpoon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/// Return harpoon back to gun if the enemies it is stuck to dies
	if (bStuckToEnemy && !IsValid(HarpoonedEnemy))
	{
		ReturnToPlayer();
	}

	/// Vector from player to harpoon
	FVector toHarpoon = GetActorLocation() - PlayerCharacter->GetActorLocation();
	FVector toHarpoonNormal = toHarpoon.GetSafeNormal();

	/// Set up the cable visually to tile the material
	VisualCableLength = FVector::Distance(GetActorLocation(), PlayerCharacter->GetActorLocation());
	float tileMaterial = VisualCableLength / 30.f;
	if (!FMath::IsNearlyEqual(tileMaterial, PrevTileMaterial, 0.01f))
	{
		CableComponent->TileMaterial = tileMaterial;
		CableComponent->MarkRenderStateDirty();
		PrevTileMaterial = tileMaterial;
	}

	/// Set rope cable length
	CableComponent->CableLength = VisualCableLength;

	/// Cache player height
	PrevPlayerHeight = CurrentPlayerHeight;
	CurrentPlayerHeight = PlayerCharacter->GetActorLocation().Z;

	/// Select harpoon mode
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
	bSwingingPlayerLastFrame = false;

	if (PlayerCharacter)
	{
		if (USkeletalMeshComponent* Mesh = PlayerCharacter->GetMesh())
		{
			if (UPlayerAnimation* Anim = Cast<UPlayerAnimation>(Mesh->GetAnimInstance()))
			{
				Anim->SetHarpoonAttached(false);
			}
		}
	}

	// Stop any projectile physics so manual movement takes over
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->Deactivate();
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FVector toSocket = (GetActorLocation() - PlayerCharacter->GetMesh()->GetSocketLocation("HarpoonGunBaseSocket")).
		GetSafeNormal();

	// Move the harpoon back toward the player at a constant return speed
	SetActorLocation(GetActorLocation() - toSocket * ReturnSpeed * DeltaTime);

	// If the harpoon is close enough to the player destroy this instance
	if (ToHarpoon.Size() < 300.f)
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
	if (!bSwingingPlayerLastFrame)
	{
		OnSwingingPlayer();
		bSwingingPlayerLastFrame = true;
	}

	// If the harpoon is stuck in the world (but not attached to an enemy)
	if (bStuck && !bStuckToEnemy)
	{
		// If player is grounded and rope is stretched beyond its cable length,
		// apply a pulling force toward the anchor to enforce rope constraint
		if (!PlayerCharacterMovementComponent->IsFalling() && ToHarpoon.Size() > CableLength)
		{
			float mult = (ToHarpoon.Size() / CableLength);
			if (mult > 1.1) { mult *= 2; }
			PlayerCharacterMovementComponent->Velocity += ToHarpoonNormal * PlayerCharacter->GetMaxWalkSpeed() * 13.7f *
				DeltaTime * mult;
		}
		else if (!PlayerCharacterMovementComponent->IsFalling())
		{
		}
		else if (ToHarpoon.Size() > CableLength || bFirstSwing)
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

			// Compute frame-to-frame harpoon displacement
			FVector HarpoonDelta = GetActorLocation() - PreviousAnchorLocation;

			// Pull player into harpoon if the harpoon is moving away
			if (!HarpoonDelta.IsNearlyZero(0.01f))
			{
				float mult = (ToHarpoon.Size() / CableLength);
				if (mult > 1.1f) { mult *= 2.0f; }

				tangentialVel += ToHarpoonNormal * PlayerCharacter->GetMaxWalkSpeed() * 13.7f * DeltaTime * mult;
			}

			// Update previous location for next frame
			PreviousAnchorLocation = GetActorLocation();


			// Apply tangential velocity to maintain swinging motion
			if (!tangentialVel.IsNearlyZero())
			{
				PlayerCharacterMovementComponent->Velocity = tangentialVel;
				//PlayerCharacter->LaunchCharacter(tangentialVel,true,  true );
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
	bSwingingPlayerLastFrame = false;
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
			if (!HarpoonedEnemy->SetActorLocation(
				HarpoonedEnemy->GetActorLocation() - ToHarpoonNormal * EnemyPullStrength * DeltaTime, true))
			{
				// If blocked , fallback to horizontal-only pull
				HarpoonedEnemy->SetActorLocation(
					HarpoonedEnemy->GetActorLocation() - FVector(ToHarpoon.X, ToHarpoon.Y, 0.f).GetSafeNormal() *
					EnemyPullStrength * DeltaTime, true);
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
		PlayerCharacter->LaunchCharacter(ToHarpoonNormal * ZipPullStrength * DeltaTime, true, true);

		// Update cable length as player moves
		CableLength = FVector::Distance(GetActorLocation(), PlayerCharacter->GetActorLocation());
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
