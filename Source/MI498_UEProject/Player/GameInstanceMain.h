// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstanceMain.generated.h"

class UScoringData;
/**
 * 
 */
UCLASS()
class MI498_UEPROJECT_API UGameInstanceMain : public UGameInstance
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
	UScoringData* ScoringData;
	
	// Mouse sensitivity, editable and accessible in Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MouseSens = 1.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool GameRestarted = false;
	
	/// Resets anything that needs to be reset on game restart
	UFUNCTION(BlueprintCallable)
	void ResetGame();
	
};
