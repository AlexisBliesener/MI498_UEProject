// Fill out your copyright notice in the Description page of Project Settings.


#include "RunAfterCutsceneFinished.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Player/GameInstanceMain.h"
URunAfterCutsceneFinished* URunAfterCutsceneFinished::RunAfterCutsceneFinished(const UObject* WorldContextObject)
{
	URunAfterCutsceneFinished* action = NewObject<URunAfterCutsceneFinished>();
	action->WorldContext = WorldContextObject;
	// the object will not be destroyed until the SetReadyToDestroy called 
	action->RegisterWithGameInstance(WorldContextObject);
	return action;
}

void URunAfterCutsceneFinished::Activate()
{
	if (!WorldContext)
	{
		OnFinished.Broadcast();
		SetReadyToDestroy();
		return;
	}

	if (UGameInstanceMain* gameInstance = Cast<UGameInstanceMain>(UGameplayStatics::GetGameInstance(WorldContext)))
	{
		if (gameInstance->bIsIntroCutScenePlayed)
		{
			OnFinished.Broadcast();
			SetReadyToDestroy();
			return;
		}

		// we want to subscribe to the cutscene when it's not played only if the intro cutscene is not played 
		gameInstance->OnCutSceneStopped.AddDynamic(this, &URunAfterCutsceneFinished::CutsceneCompleted);
	}
	else
	{
		OnFinished.Broadcast();
		SetReadyToDestroy();
	}
}

void URunAfterCutsceneFinished::CutsceneCompleted()
{
	if (WorldContext)
	{
		if (UGameInstanceMain* gameInstance = Cast<UGameInstanceMain>(UGameplayStatics::GetGameInstance(WorldContext)))
		{
			gameInstance->OnCutSceneStopped.RemoveDynamic(this, &URunAfterCutsceneFinished::CutsceneCompleted);
		}
	}

	OnFinished.Broadcast();

	SetReadyToDestroy();
}
