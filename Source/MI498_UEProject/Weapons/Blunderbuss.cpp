#include "Blunderbuss.h"
#include "../Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

/// Implements the Blunderbuss' primary attack
/// This function handles hitscan firing, camera recoil, and knockback
/// @param PlayerController - The player who is firing the weapon
void ABlunderbuss::PrimaryAttack(APlayerController* PlayerController)
{
	Super::PrimaryAttack(PlayerController);
	
	/// Get the player camera location and rotation for aiming
	FVector cameraLocation;
	FRotator cameraRotation;
	PlayerController->GetPlayerViewPoint(cameraLocation, cameraRotation);
	
	/// Calculate the end location of the trace based on weapon range
	FVector cameraForwardVector = cameraRotation.Vector();
	
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

void ABlunderbuss::SecondaryAttack(APlayerController* PlayerController)
{
}

