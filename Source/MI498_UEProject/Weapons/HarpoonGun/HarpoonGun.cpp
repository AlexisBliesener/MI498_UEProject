#include "HarpoonGun.h"

#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"

void AHarpoonGun::PrimaryAttack(AController* Controller,AActor* Target)
{
	Super::PrimaryAttack(Controller, Target);

	if (APlayerController* playerController = Cast<APlayerController>(Controller) )
	{
		/// Get the player camera location and rotation for accurate aiming
		FVector CameraLocation;
		FRotator CameraRotation;
		playerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		/// Configure spawn parameters for ownership and instigation
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = playerController->GetPawn();
	
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
}

void AHarpoonGun::PrimaryAttackHold(AController* Controller, AActor* Target)
{
	/// No functionality
}

void AHarpoonGun::SecondaryAttack(AController* Controller,AActor* Target)
{
	Super::SecondaryAttack(Controller);
}

void AHarpoonGun::SecondaryAttackHoldStart(AController* Controller, AActor* Target)
{
	Super::SecondaryAttackHoldStart(Controller, Target);
	
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	playerCharacter->SetOverrideCameraFOV(true,  ADSFOV);
}

void AHarpoonGun::SecondaryAttackHoldEnd(AController* Controller, AActor* Target)
{
	Super::SecondaryAttackHoldEnd(Controller, Target);
	
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	playerCharacter->SetOverrideCameraFOV(false);
}

void AHarpoonGun::DestroyCurrentHarpoon()
{
	CurrentHarpoon->Destroy();
	CurrentHarpoon = nullptr;
}
