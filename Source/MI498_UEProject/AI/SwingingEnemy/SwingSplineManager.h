// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwingSpline.h"
#include "SwingSplineManager.generated.h"

class ASwingingEnemy;
/**
 * A class manages the swing splines in the level
 * It helps the swinging enemies find a good spline to use, and it can stop two enemies from using the same spline at the same time
 */
UCLASS()
class MI498_UEPROJECT_API ASwingSplineManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASwingSplineManager();
	
	/// All spline points on this manager 
	UPROPERTY(EditAnywhere, Category="Swing")
	TArray<TObjectPtr<ASwingSpline>> Splines;

	/**
	 * Get the closest available spline point from the requester (enemy) with a Query Extent
	 * reserve points are different from use point as the reserve can be in a large distance  
	 * @param Requester the enemy 
	 * @param FromLocation the location of the enemy
	 * @param bIsReserve for this request is it to reserver or to use? 
	 * @return ASwingSpline pointer if spline found, otherwise null ptr
	 */
	ASwingSpline* RequestClosestAvailableSpline(ASwingingEnemy* Requester,const FVector& FromLocation,const bool bIsReserve);
	
};
