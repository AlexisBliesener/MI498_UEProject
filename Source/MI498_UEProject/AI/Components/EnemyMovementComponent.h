// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MI498_UEPROJECT_API UEnemyMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UEnemyMovementComponent();
	virtual FVector GetActorFeetLocation() const override;

	virtual void RequestPathMove(const FVector& MoveInput) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
