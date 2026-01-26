#include "Sword.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"

void ASword::PrimaryAttack(APlayerController* PlayerController)
{
	Super::PrimaryAttack(PlayerController);
}

void ASword::SecondaryAttack(APlayerController* PlayerController)
{
	/// Get the player camera location and rotation for dash direction
	FVector cameraLocation;
	FRotator cameraRotation;
	PlayerController->GetPlayerViewPoint(cameraLocation, cameraRotation);
	FVector cameraForwardVector = cameraRotation.Vector();
	
	/// Get a reference to the owning player character
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner());
	
	/// Dashes the player forward in look direction
	FVector launchVelocity = cameraForwardVector * DashForce;
	playerCharacter->LaunchCharacter(launchVelocity, true, true);
}