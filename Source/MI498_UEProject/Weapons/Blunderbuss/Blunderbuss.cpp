#include "Blunderbuss.h"
#include "../../Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

/// Implements the Blunderbuss' primary attack
/// This function handles hitscan firing, camera recoil, and knockback
/// @param Controller - The player who is firing the weapon
/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
void ABlunderbuss::PrimaryAttack(AController* Controller,AActor* Target)
{
	Super::PrimaryAttack(Controller);
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		/// Get the player camera location and rotation for aiming
		FVector cameraLocation;
		FRotator cameraRotation;
		playerController->GetPlayerViewPoint(cameraLocation, cameraRotation);
	
		/// Calculate the end location of the trace based on weapon range
		FVector cameraForwardVector = cameraRotation.Vector();
	
		/// Apply camera recoil to simulate weapon kickback
		playerController->AddPitchInput(CameraRecoil);
	
		/// Apply physical recoil to the player if airborne
		APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner());
		if (!playerCharacter->GetCharacterMovement()->IsMovingOnGround())
		{
			/// Launch the player backward based on knockback force and firing direction
			FVector launchVelocity = -cameraForwardVector * KnockbackForce;
			playerCharacter->LaunchCharacter(launchVelocity, false, false);
		}	
	}
}

void ABlunderbuss::SecondaryAttack(AController* Controller,AActor* Target)
{
}

