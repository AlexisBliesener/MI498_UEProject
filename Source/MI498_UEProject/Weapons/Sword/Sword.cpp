#include "Sword.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"

void ASword::PrimaryAttack(AController* Controller, AActor* Target)
{
	Super::PrimaryAttack(Controller);
}

void ASword::SecondaryAttack(AController* Controller,AActor* Target)
{
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		/// Get the player camera location and rotation for dash direction
		FVector cameraLocation;
		FRotator cameraRotation;
		playerController->GetPlayerViewPoint(cameraLocation, cameraRotation);
		FVector cameraForwardVector = cameraRotation.Vector();
	
		/// Get a reference to the owning player character
		APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner());
	
		/// Dashes the player forward in look direction
		FVector launchVelocity = cameraForwardVector * DashForce;
		playerCharacter->LaunchCharacter(launchVelocity, true, true);
	}
}