// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstanceMain.generated.h"

/**
 * 
 */
UCLASS()
class MI498_UEPROJECT_API UGameInstanceMain : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// Mouse sensitivity, editable and accessible in Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MouseSens = 1.0f;
	
};
