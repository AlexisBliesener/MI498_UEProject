#include "CloudSpawner.h"

#include "NiagaraComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/DecalComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

ACloudSpawner::ACloudSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACloudSpawner::BeginPlay()
{
	Super::BeginPlay();

	SkySphereStartingZ = SkySphere->GetActorLocation().Z;

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("ShipExplosion"), FoundExplosions);
}

void ACloudSpawner::Reset()
{
	/// Destroy all clouds
	for (int32 i = SpawnedClouds.Num() - 1; i >= 0; i--)
	{
		AActor* Cloud = SpawnedClouds[i];
		if (!IsValid(Cloud)) continue;
		Cloud->Destroy();
		CloudSpeeds.Remove(Cloud);
		SpawnedClouds.RemoveAt(i);
	}

	/// Stop the cloud spawning timer
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);

	/// Reset sky location
	FVector Location = SkySphere->GetActorLocation();
	Location.Z = SkySphereStartingZ;
	SkySphere->SetActorLocation(Location);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	for (AActor* Actor : FoundExplosions)
	{
		if (!IsValid(Actor)) continue;

		TWeakObjectPtr<UNiagaraComponent> NiagaraComp = Actor->FindComponentByClass<UNiagaraComponent>();
		TWeakObjectPtr<UDecalComponent> DecalComp = Actor->FindComponentByClass<UDecalComponent>();
		
		if (NiagaraComp.IsValid())
		{
			NiagaraComp->Activate(false);
		}
		
		if (DecalComp.IsValid())
		{
			DecalComp->SetVisibility(false);
		}
	}


	/// Set cloud spawner inactive
	bActive = false;
}

void ACloudSpawner::Activate()
{
	/// Start cloud spawning timer
	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimer,
		this,
		&ACloudSpawner::SpawnCloud,
		SpawnRate,
		true);

	/// Set cloud spawner active
	bActive = true;
	
	TriggerShipExplosions();
}

void ACloudSpawner::SpawnCloud()
{
	if (!bActive) return;

	/// Pick random spawn location
	FVector Min = Corner1.ComponentMin(Corner2);
	FVector Max = Corner1.ComponentMax(Corner2);
	FVector SpawnLocation = FMath::RandPointInBox(FBox(Min, Max));
	SpawnLocation += GetActorLocation();

	// Random rotation
	FRotator RandomRotation;
	RandomRotation.Yaw = FMath::RandRange(0.f, 360.f);
	RandomRotation.Pitch = FMath::RandRange(-5.f, 5.f);
	RandomRotation.Roll = FMath::RandRange(-5.f, 5.f);

	// Spawn
	AActor* Cloud = GetWorld()->SpawnActor<AActor>(CloudClass, SpawnLocation, RandomRotation);

	if (!IsValid(Cloud)) return;

	// Random scale
	float Scale = FMath::RandRange(MinScale.X, MaxScale.X);
	FVector RandomScale = FVector(Scale);
	Cloud->SetActorScale3D(RandomScale);

	SpawnedClouds.Add(Cloud);

	float Speed = FMath::RandRange(MinSpeed, MaxSpeed);
	CloudSpeeds.Add(Cloud, Speed);
}

void ACloudSpawner::Tick(float DeltaTime)
{
	if (!bActive) return;

	Super::Tick(DeltaTime);

	/// Move clouds
	for (int32 i = SpawnedClouds.Num() - 1; i >= 0; i--)
	{
		AActor* Cloud = SpawnedClouds[i];

		if (!IsValid(Cloud))
		{
			SpawnedClouds.RemoveAt(i);
			continue;
		}

		float* SpeedPtr = CloudSpeeds.Find(Cloud);

		FVector Location = Cloud->GetActorLocation();
		Location.Z += (*SpeedPtr) * DeltaTime;
		Cloud->SetActorLocation(Location);

		if (Location.Z > MaxHeight + GetActorLocation().Z)
		{
			Cloud->Destroy();
			CloudSpeeds.Remove(Cloud);
			SpawnedClouds.RemoveAt(i);
		}
	}

	/// Move sky sphere
	if (SkySphere)
	{
		FVector Location = SkySphere->GetActorLocation();

		// move upward slowly
		Location.Z += 50.f * DeltaTime;

		SkySphere->SetActorLocation(Location);
	}
}

void ACloudSpawner::TriggerShipExplosions()
{
	if (!bActive) return;

	for (AActor* Actor : FoundExplosions)
	{
		if (!IsValid(Actor)) continue;

		TWeakObjectPtr<UNiagaraComponent> NiagaraComp = Actor->FindComponentByClass<UNiagaraComponent>();
		TWeakObjectPtr<UDecalComponent> DecalComp = Actor->FindComponentByClass<UDecalComponent>();

		float Delay = FMath::RandRange(0.3f, 3.0f);

		FTimerDelegate Delegate;
		Delegate.BindWeakLambda(this, [this, NiagaraComp, DecalComp]()
		{
			if (NiagaraComp.IsValid())
			{
				NiagaraComp->Activate(true);
			}
			
			if (DecalComp.IsValid())
			{
				DecalComp->SetVisibility(true);
			}
		});

		FTimerHandle LocalHandle;
		GetWorld()->GetTimerManager().SetTimer(
			LocalHandle,
			Delegate,
			Delay,
			false
		);
	}
}
