// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SyncTransformOnHiddenShipComponent.generated.h"


/**
 * Use this component if you want to update the nav mesh on the hidden ship for that actor,
 * this component will be on the actor that in the real ship 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MI498_UEPROJECT_API USyncTransformOnHiddenShipComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USyncTransformOnHiddenShipComponent();
	
	/// the actor on the hidden ship 
	UPROPERTY(BlueprintReadOnly)
	AActor* HiddeActor;
	
	/**
	 * You can call this function whenever the object position is changed so it can copy 
	 * the transform from the real ship into the fake so the AI nav mesh gets updated
	 * @param SceneComponentToUpdate new transform 
	 */
	UFUNCTION(BlueprintCallable)
	void SyncHiddenTransform(USceneComponent* SceneComponentToUpdate);
};
