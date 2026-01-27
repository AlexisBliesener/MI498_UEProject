#include "HarpoonGun.h"

void AHarpoonGun::PrimaryAttack(APlayerController* PlayerController)
{
	OnPrimaryAttack();

	/// Get the player camera location and rotation for accurate aiming
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	/// Configure spawn parameters for ownership and instigation
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = PlayerController->GetPawn();
	
	/// Destroy the existing harpoon if one is already active
	if (CurrentHarpoon != nullptr)
	{
		DestroyCurrentHarpoon();
	}
	
	/// Spawn the harpoon slightly in front of the camera to avoid self-collision
	CurrentHarpoon = GetWorld()->SpawnActor<AHarpoon>(HarpoonBlueprint, CameraLocation + CameraRotation.Vector() * 200, CameraRotation, SpawnParams);
	
	/// Initialize harpoon properties after spawning
	CurrentHarpoon->SetRange(Range);
	CurrentHarpoon->SetHarpoonGun(this);
}

void AHarpoonGun::SecondaryAttack(APlayerController* PlayerController)
{
	Super::SecondaryAttack(PlayerController);
}

void AHarpoonGun::DestroyCurrentHarpoon()
{
	CurrentHarpoon->Destroy();
	CurrentHarpoon = nullptr;
}
