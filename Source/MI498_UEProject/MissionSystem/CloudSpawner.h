#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CloudSpawner.generated.h"

UCLASS()
class MI498_UEPROJECT_API ACloudSpawner : public AActor
{
	GENERATED_BODY()

public:
	ACloudSpawner();
	void BeginPlay();
	
	/// Resets the clouds and the sky moving up
	void Reset();
	
	/// Activates cloud spawn and sky move up
	void Activate();

protected:
	virtual void Tick(float DeltaTime) override;
	
	// Skysphere
	UPROPERTY(EditAnywhere, Category = "Clouds")
	AActor* SkySphere;

	// Cloud class to spawn
	UPROPERTY(EditAnywhere, Category = "Clouds")
	TSubclassOf<AActor> CloudClass;

	// Spawn area 
	UPROPERTY(EditAnywhere, Category = "Clouds", meta = (MakeEditWidget = "true"))
	FVector Corner1;

	UPROPERTY(EditAnywhere, Category = "Clouds", meta = (MakeEditWidget = "true"))
	FVector Corner2;

	// Movement speed range
	UPROPERTY(EditAnywhere, Category = "Clouds")
	float MinSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = "Clouds")
	float MaxSpeed = 50.f;
	
	/// Scale range
	UPROPERTY(EditAnywhere, Category = "Clouds")
	FVector MinScale = FVector(300);

	UPROPERTY(EditAnywhere, Category = "Clouds")
	FVector MaxScale = FVector(500);

	// Spawn timing
	UPROPERTY(EditAnywhere, Category = "Clouds")
	float SpawnRate = 1.f;

	// Despawn height
	UPROPERTY(EditAnywhere, Category = "Clouds")
	float MaxHeight = 1000.f;

private:
	/// Hangles when a new cloud will be spawned
	FTimerHandle SpawnTimer;

	/// Track clouds + their speeds
	TArray<AActor*> SpawnedClouds;
	TMap<AActor*, float> CloudSpeeds;
	
	/// The starting location of the sky sphere
	float SkySphereStartingZ = 0;
	
	/// Spawns a new cloud
	void SpawnCloud();
	
	/// If the cloud spawner is currently active
	bool bActive = false;
};