// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "MI498_UEProject/Characters/Enemies/SwingingEnemy/SwingingEnemy.h"
#include "SwingSpline.generated.h"


/**
 * a spline actor used for “swing” movement for the swinging enemy 
 */
UCLASS()
class MI498_UEPROJECT_API ASwingSpline : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASwingSpline();
	/// Current spline for this swingspline class
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;
	
	/**
	 * Is this spline available? 
	 * Available when the owner of this spline is the same as the requested
	 * Or this spline is empty 
	 * @param Enemy the requester 
	 * @return true if available
	 */
	UFUNCTION(BlueprintCallable)
	bool IsAvailable(ASwingingEnemy* Enemy) const;
	/**
	 * This will reserve the spline and assign it to the enemy 
	 * @param Enemy the requester 
	 */
	UFUNCTION(BlueprintCallable)
	void UseSpline(ASwingingEnemy* Enemy);
	/**
	 * This will release the spline and unassign it to the enemy 
	 * @param Enemy the requester 
	 */
	UFUNCTION(BlueprintCallable)
	void ReleaseSpline(ASwingingEnemy* Enemy);
	/// The stop time after the enemy reach to the end of this spline 
	UPROPERTY(EditAnywhere)
	float StopTime = 0.f;
	/**
	 * Get the start location of the spline (at index 1..) 
	 * @return The Start location of the spline 
	 */
	UFUNCTION(BlueprintCallable)
	FVector GetStartLocation() const;

private:
	/// The owner of this spline (if there is any) 
	UPROPERTY()
	TObjectPtr<ASwingingEnemy> CurrentEnemy;
};
