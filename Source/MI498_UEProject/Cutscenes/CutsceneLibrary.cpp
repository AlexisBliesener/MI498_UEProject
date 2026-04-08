// Fill out your copyright notice in the Description page of Project Settings.


#include "CutsceneLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Player/GameInstanceMain.h"

void UCutsceneLibrary::CheckIfCutsceneFinished(const UObject* WorldContextObject, ECutsceneCheck& OutPin)
{
	if (WorldContextObject)
	{
		if (UGameInstanceMain* gameInstance = Cast<UGameInstanceMain>(UGameplayStatics::GetGameInstance(WorldContextObject)))
		{
			if (gameInstance->bIsIntroCutScenePlayed)
			{
				OutPin = ECutsceneCheck::FinishedOrPlayedBefore;
				return;
			}
		}
	}

	OutPin = ECutsceneCheck::NotFinished;
}

void UCutsceneLibrary::CheckIfCutsceneSkipped(const UObject* WorldContextObject, ECutsceneSkipped& OutPin)
{
	if (WorldContextObject)
	{
		if (UGameInstanceMain* gameInstance = Cast<UGameInstanceMain>(UGameplayStatics::GetGameInstance(WorldContextObject)))
		{
			if (gameInstance->bIsIntroCutSceneSkipped)
			{
				OutPin = ECutsceneSkipped::Skipped;
				return;
			}
		}
	}

	OutPin = ECutsceneSkipped::NotSkipped;
}
