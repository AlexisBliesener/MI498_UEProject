// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VaultTreasure.generated.h"

UCLASS()
class MI498_UEPROJECT_API AVaultTreasure : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> LootToShrink;
	
	virtual void BeginPlay() override;
	
};
