// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeEnemyComponent.h"


UStateTreeEnemyComponent::UStateTreeEnemyComponent()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bTickEvenWhenPaused = true; 
	SetStartLogicAutomatically(false);
	
	UE_LOG(LogTemp, Error, TEXT("UStateTreeEnemyComponent initializieid!"));
	// ...
}

void UStateTreeEnemyComponent::StartStateTree(UStateTree* StateTree)
{
	UE_LOG(LogTemp, Error, TEXT("BEGIN!!"));
	if (StateTree)
	{
		if (IsRunning())
		{
			Cleanup();
		}

		StateTreeRef.SetStateTree(StateTree);
		Super::SetStateTree(StateTree);
		StartLogic();
		UE_LOG(LogTemp, Error, TEXT("STARTEDF!!"));
	}
}

void UStateTreeEnemyComponent::StopStateTree()
{
	if (IsRunning())
	{
		Cleanup();
	}
    
	StopLogic(FString("Code"));
	Super::SetStateTree(nullptr);
	StateTreeRef.SetStateTree(nullptr);
}


// Called when the game starts
void UStateTreeEnemyComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UStateTreeEnemyComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

