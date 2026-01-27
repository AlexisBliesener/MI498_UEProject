#include "WeaponBase.h"

DEFINE_LOG_CATEGORY(WeaponLog);

void AWeaponBase::PrimaryAttack(APlayerController* PlayerController)
{
	OnPrimaryAttack();

	/// Get the player camera location and rotation for aiming
	FVector cameraLocation;
	FRotator cameraRotation;
	PlayerController->GetPlayerViewPoint(cameraLocation, cameraRotation);
	
	/// Prepare a hit result to store the outcome of the line trace
	FHitResult hitResult;
	
	/// Calculate the end location of the trace based on weapon range
	FVector cameraForwardVector = cameraRotation.Vector();
	FVector endLocation = cameraLocation + cameraForwardVector * Range;
	
	/// Setup collision parameters for the trace
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(GetOwner());
	
	/// Perform a hitscan trace from the camera forward
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, cameraLocation,endLocation, ECC_Visibility, TraceParams);
	
	/// Draw a debug line showing the trace in the world
	DrawDebugLine(
	GetWorld(),
	cameraLocation,
	bHit ? hitResult.ImpactPoint : endLocation,
	FColor::Red,
	false,
	1.f,
	0,
	1.f
	);
	
	/// TODO: Check if HitResult hit an enemy and apply damage
}

void AWeaponBase::SecondaryAttack(APlayerController* PlayerController)
{
}
