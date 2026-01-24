#include "Blunderbuss.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

/// Implements the Blunderbuss' primary attack
/// This function handles hitscan firing, camera recoil, and knockback
/// @param PlayerController - The player who is firing the weapon
void ABlunderbuss::PrimaryAttack(APlayerController* PlayerController)
{
	/// Get the player camera location and rotation for aiming
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	
	/// Prepare a hit result to store the outcome of the line trace
	FHitResult HitResult;
	
	/// Calculate the end location of the trace based on weapon range
	FVector CameraForwardVector = CameraRotation.Vector();
	FVector EndLocation = CameraLocation + CameraForwardVector * Range;
	
	/// Setup collision parameters for the trace
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(GetOwner());
	
	/// Perform a hitscan trace from the camera forward
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation,EndLocation, ECC_Visibility, TraceParams);
	
	/// Draw a debug line showing the trace in the world
	DrawDebugLine(
	GetWorld(),
	CameraLocation,
	bHit ? HitResult.ImpactPoint : EndLocation,
	FColor::Red,
	false,
	1.f,
	0,
	1.f
	);
	
	/// TODO: Check if HitResult hit an enemy and apply damage
	
	/// Apply camera recoil to simulate weapon kickback
	PlayerController->AddPitchInput(CameraRecoil);
	
	/// Apply physical recoil to the player if airborne
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!PlayerCharacter->GetCharacterMovement()->IsMovingOnGround())
	{
		/// Launch the player backward based on knockback force and firing direction
		FVector LaunchVelocity = -CameraForwardVector * KnockbackForce;
		PlayerCharacter->LaunchCharacter(LaunchVelocity, false, false);
	}
}

