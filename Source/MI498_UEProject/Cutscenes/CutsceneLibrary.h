// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CutsceneLibrary.generated.h"


UENUM(BlueprintType)
enum class ECutsceneCheck : uint8
{
	NotFinished,
	FinishedOrPlayedBefore
};
UENUM(BlueprintType)
enum class ECutsceneSkipped : uint8
{
	NotSkipped,
	Skipped
};
/**
 * this will have 
 */
UCLASS()
class MI498_UEPROJECT_API UCutsceneLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * Check if the intro cutscene has played/finished.
	 * @param WorldContextObject WorldContextObject
	 * @param OutPin which pin is executed 
	 */
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "OutPin", WorldContext = "WorldContextObject"))
	static void CheckIfCutsceneFinished(const UObject* WorldContextObject, ECutsceneCheck& OutPin);
	/**
	 * Check if the intro cutscene has skipped.
	 * @param WorldContextObject WorldContextObject
	 * @param OutPin which pin is executed 
	 */
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "OutPin", WorldContext = "WorldContextObject"))
	static void CheckIfCutsceneSkipped(const UObject* WorldContextObject, ECutsceneSkipped& OutPin);
};
