// Fill out your copyright notice in the Description page of Project Settings.


#include "BombProjectile.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"


ABombProjectile::ABombProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(20.f); 
	RootComponent = Collision;

	Collision->SetCollisionResponseToAllChannels(ECR_Block);
    
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); 
    
	Collision->SetNotifyRigidBodyCollision(true);

	Collision->OnComponentHit.AddDynamic(this, &ABombProjectile::OnHit);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->UpdatedComponent = Collision;
	Movement->InitialSpeed = 0.f;
	Movement->MaxSpeed = 5000.f;
	Movement->ProjectileGravityScale = 1.f;
	Movement->bRotationFollowsVelocity = true;
	Movement->bShouldBounce = false;
	Movement->bAutoActivate = false;
	Movement->bSweepCollision = true; 

}
void ABombProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, FVector NormalImpulse,
							const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(GetOwner());
		
		UGameplayStatics::ApplyRadialDamage(
			GetWorld(),
			Damage,
			GetActorLocation(),
			ExplosionRadius,
			UDamageType::StaticClass(),
			ActorsToIgnore,    
			this,
			GetInstigatorController(),
			true
		);
		SetActorScale3D(FVector::ZeroVector);
		
		if (ExplosionVFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionVFX,GetActorLocation(),FRotator(0.f, 90.f, 0.f), FVector(0.2f),true,true,ENCPoolMethod::None,true);
		}
		OnExplode();
		
		Destroy();
	}
}


void ABombProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	OnThrown();
}

float ABombProjectile::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (DamageCauser == GetOwner())
	{
		return 0.f;
	}
	
	if (ExplosionVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionVFX,GetActorLocation(),FRotator(0.f, 90.f, 0.f), FVector(0.2f),true,true,ENCPoolMethod::None,true);
	}
	OnExplode();
	
    // If player shoots the bomb early it explodes mid air
	Destroy(); 

	return DamageAmount;
}

void ABombProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

