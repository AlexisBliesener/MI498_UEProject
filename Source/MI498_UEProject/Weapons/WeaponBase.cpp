#include "WeaponBase.h"

#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(WeaponLog);

void AWeaponBase::PrimaryAttack(AController* PlayerController, AActor* Target)
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
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, cameraLocation,endLocation, ECC_Pawn, TraceParams);
	
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
	
	/// Check if HitResult hit an enemy and apply damage
	if (bHit && hitResult.GetActor())
	{
		
		UGameplayStatics::ApplyDamage(
			hitResult.GetActor(),
			Damage, // weapon damage
			PlayerController,
			this,
			nullptr
		);
	}
}

void AWeaponBase::SecondaryAttack(AController* Controller,AActor* Target)
{
}
