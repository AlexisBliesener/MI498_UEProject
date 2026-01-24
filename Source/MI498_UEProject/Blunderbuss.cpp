#include "Blunderbuss.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

/// Implements the Blunderbuss' primary attack
/// This function handles hitscan firing, camera recoil, and knockback
/// @param PlayerController - The player who is firing the weapon
void ABlunderbuss::PrimaryAttack(APlayerController* PlayerController)
{
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
	
	/// Apply camera recoil to simulate weapon kickback
	PlayerController->AddPitchInput(CameraRecoil);
	
	/// Apply physical recoil to the player if airborne
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!playerCharacter->GetCharacterMovement()->IsMovingOnGround())
	{
		/// Launch the player backward based on knockback force and firing direction
		FVector launchVelocity = -cameraForwardVector * KnockbackForce;
		playerCharacter->LaunchCharacter(launchVelocity, false, false);
	}
}

