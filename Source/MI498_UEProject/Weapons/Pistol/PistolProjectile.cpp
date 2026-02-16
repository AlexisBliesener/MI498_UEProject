// Fill out your copyright notice in the Description page of Project Settings.


#include "PistolProjectile.h"

#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"


APistolProjectile::APistolProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(6.f);
	RootComponent = Collision;
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Collision->SetGenerateOverlapEvents(true);
	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->UpdatedComponent = Collision;
	Movement->InitialSpeed = 3000.f;
	Movement->MaxSpeed = 3000.f;
	Movement->bRotationFollowsVelocity = true;
	Movement->ProjectileGravityScale = 0.f;
	Collision->OnComponentHit.AddDynamic(this, &APistolProjectile::OnHit);
}

void APistolProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// don't hit the char itself and the owner (the projectile)
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner()) 
	{
		return;
	}
	
	
	if (OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		UGameplayStatics::ApplyDamage(
			OtherActor,
			Damage,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);
	}
	
	
	Destroy();
}

void APistolProjectile::BeginPlay()
{
	
	InitialLifeSpan = DestroyAfterTime; // This should be called before Super::BeginPlay!! why? because unreal said
	
	Super::BeginPlay();
	
	// ignore the bullet to hits 
	if (AActor* owner = GetOwner())
	{
		Collision->IgnoreActorWhenMoving(owner, true);
	}
}

void APistolProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

