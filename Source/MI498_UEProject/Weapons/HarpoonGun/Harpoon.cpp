#include "Harpoon.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CableComponent.h"
#include "HarpoonGun.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"


AHarpoon::AHarpoon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	/// Collision sphere used to detect impacts
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(5.f);
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

void AHarpoon::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	/// Ignore further hits if already stuck, invalid actor, or hitting the owning player
	if (Stuck || !OtherActor || OtherActor->GetUniqueID() == GetOwner()->GetOwner()->GetUniqueID())
	{
		return;
	}
	
	/// Stop projectile movement when the harpoon sticks
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->Deactivate();
	
	/// Attach the cable end to the owning player's root component
	CableComponent->SetAttachEndToComponent(GetOwner()->GetOwner()->GetRootComponent());
	
	/// Store the initial rope length when the harpoon hits
	CableLength = FVector::Distance(Hit.ImpactPoint, GetOwner()->GetOwner()->GetActorLocation());
	
	/// Snap harpoon to the impact point and mark as stuck
	SetActorLocation(Hit.ImpactPoint);
	Stuck = true;
}

void AHarpoon::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner()->GetOwner());
}

void AHarpoon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	/// Vector from player to harpoon
	FVector toPlayer = GetActorLocation() - PlayerCharacter->GetActorLocation();
	
	if (Stuck)
	{
		/// Enforce rope length by pulling the player back if they exceed it
		if (toPlayer.Size() > CableLength)
		{
			PlayerCharacter->LaunchCharacter(toPlayer.GetSafeNormal() * PullStrength * DeltaTime, false, false);
		}
	}
	else
	{
		/// Destroy the harpoon if it exceeds its maximum range without hitting
		if (toPlayer.Size() > Range)
		{
			HarpoonGun->DestroyCurrentHarpoon();
		}
	}
}
