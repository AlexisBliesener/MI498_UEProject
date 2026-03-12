// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMovementComponent.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"

// Sets default values for this component's properties
UEnemyMovementComponent::UEnemyMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
}

// Called every frame
void UEnemyMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

FVector UEnemyMovementComponent::GetActorFeetLocation() const
{
	FVector realFeetLoc = Super::GetActorFeetLocation();
    
	AEnemyBase* enemy = Cast<AEnemyBase>(GetOwner());
	if (enemy && enemy->RealShip && enemy->HiddenShip)
	{
		// Change real location to hidden location
		FVector localPos = enemy->RealShip->GetActorTransform().InverseTransformPosition(realFeetLoc);
		FVector hiddenFeetLoc = enemy->HiddenShip->GetActorTransform().TransformPosition(localPos);
        
		return hiddenFeetLoc;
	}

	return realFeetLoc;
}

void UEnemyMovementComponent::RequestPathMove(const FVector& MoveInput)
{
	AEnemyBase* enemy = Cast<AEnemyBase>(GetOwner());
    
	if (enemy && enemy->RealShip && enemy->HiddenShip)
	{
		FVector localPos = enemy->HiddenShip->GetActorTransform().InverseTransformVector(MoveInput);
		FVector realPos = enemy->RealShip->GetActorTransform().TransformVector(localPos);
		Super::RequestPathMove(realPos);
		return;
	}

	Super::RequestPathMove(MoveInput);
}

