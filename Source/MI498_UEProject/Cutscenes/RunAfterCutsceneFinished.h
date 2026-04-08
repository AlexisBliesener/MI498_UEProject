// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "RunAfterCutsceneFinished.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaitCutsceneFinished, bool, bHasSkipped);
/**
 *  This task is going to wait until the intro cutscene is finished,
 *  OnFinished will be trigger when the intro cutscene is finished OR it's already finished before
 */
UCLASS()
class MI498_UEPROJECT_API URunAfterCutsceneFinished : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/// OnFinished will be trigger when the intro cutscene is finished OR it's already finished before
	UPROPERTY(BlueprintAssignable)
	FOnWaitCutsceneFinished OnFinished;
	
	/**
	 * Use that if you want to wait until the intro cutscene is finished,
	 * the cutscene will be played once on BeginPlay in the level blueprint, After that the OnFinshed is called
	 * If the level is restarted and the cutscene has already played, OnFinished will still be triggered!
	 * @param WorldContextObject this is assigned automatically in the blueprint 
	 * @return the class itself; this is how unreal handles the blueprint actions... 
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static URunAfterCutsceneFinished* RunAfterCutsceneFinished(const UObject* WorldContextObject);

	virtual void Activate() override;
private:
	/// Used to get the game instance reference. 
	UPROPERTY()
	const UObject* WorldContext;
	/**
	 * Called once the cutscene is completed
	 */
	UFUNCTION()
	void CutsceneCompleted();
};
