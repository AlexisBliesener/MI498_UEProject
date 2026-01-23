// Fill out your copyright notice in the Description page of Project Settings.


#include "Blunderbuss.h"


// Sets default values
ABlunderbuss::ABlunderbuss()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABlunderbuss::Fire()
{
	GEngine->AddOnScreenDebugMessage(
		   -1,              // key (-1 = new message each time)
		   5.f,             // display time in seconds
		   FColor::Red,     // text color
		   TEXT("Blunderbuss fired!") // message
	   );
}

// Called when the game starts or when spawned
void ABlunderbuss::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlunderbuss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

