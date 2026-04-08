// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "JumpNavLinkProxy.generated.h"


UCLASS()
class MI498_UEPROJECT_API AJumpNavLinkProxy : public ANavLinkProxy
{
	GENERATED_BODY()

public:
	AJumpNavLinkProxy(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	/**
	 * This is called when the enemy reaches a jump nav link point 
	 * @param MovingActor the current actor that called the smart link proxy 
	 * @param DestinationPoint the other side of the smart link proxy 
	 */
	UFUNCTION()
	void SmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint);
};
