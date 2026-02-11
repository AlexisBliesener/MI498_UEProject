// Fill out your copyright notice in the Description page of Project Settings.


#include "SwingSpline.h"



// Sets default values
ASwingSpline::ASwingSpline()
{
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

bool ASwingSpline::IsAvailable(ASwingingEnemy* Enemy) const
{
	return CurrentEnemy == Enemy || CurrentEnemy == nullptr;
}

void ASwingSpline::UseSpline(ASwingingEnemy* Enemy)
{
	if (!CurrentEnemy)
		CurrentEnemy = Enemy;
}

void ASwingSpline::ReleaseSpline(ASwingingEnemy* Enemy)
{
	if (CurrentEnemy == Enemy)
	{
		CurrentEnemy = nullptr;
		Enemy->CurrentSpline = nullptr;
	}
}

FVector ASwingSpline::GetStartLocation() const
{
	return Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
}
