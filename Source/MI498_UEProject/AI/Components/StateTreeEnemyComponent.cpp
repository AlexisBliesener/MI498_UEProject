// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeEnemyComponent.h"


UStateTreeEnemyComponent::UStateTreeEnemyComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bTickEvenWhenPaused = true; 
	SetStartLogicAutomatically(false);
}

void UStateTreeEnemyComponent::StartStateTree(UStateTree* StateTree)
{
	if (StateTree)
	{
		if (IsRunning())
		{
			Cleanup();
		}

		StateTreeRef.SetStateTree(StateTree);
		Super::SetStateTree(StateTree);
		StartLogic();
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


void UStateTreeEnemyComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UStateTreeEnemyComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

