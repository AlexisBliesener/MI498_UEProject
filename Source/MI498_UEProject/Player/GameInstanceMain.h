// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstanceMain.generated.h"

/// Dynamic multicast delegate that is broadcast when a bomb piece is collected
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCutSceneStopped);
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
	
	/// This event will be called when the cutscene stopped  
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnCutSceneStopped OnCutSceneStopped;
	/// Has the cutscene played before?
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsIntroCutScenePlayed = true;
	/// Has the cutscene skipped?
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsIntroCutSceneSkipped = false;
};
