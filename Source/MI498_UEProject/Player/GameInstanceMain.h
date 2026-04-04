// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstanceMain.generated.h"

/// Dynamic multicast delegate that is broadcast when a bomb piece is collected
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCutSceneStopped);
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool GameRestarted = false;
	
	/// This event will be called when the cutscene stopped  
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnCutSceneStopped OnCutSceneStopped;
	/// Has the cutscene played before?
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsIntroCutScenePlayed = true;
	/// Resets anything that needs to be reset on game restart
	UFUNCTION(BlueprintCallable)
	void ResetGame();
	
};
