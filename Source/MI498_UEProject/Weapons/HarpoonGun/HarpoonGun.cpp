#include "HarpoonGun.h"

#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"

void AHarpoonGun::PrimaryAttack(AController* Controller,AActor* Target)
{
	if (CurrentHarpoon != nullptr) return;
	
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

void AHarpoonGun::PrimaryAttackHoldStart(AController* Controller, AActor* Target)
{
	Super::PrimaryAttackHoldStart(Controller, Target);
	bSwingMode = true;
}

void AHarpoonGun::PrimaryAttackHoldEnd(AController* Controller, AActor* Target)
{
	Super::PrimaryAttackHoldEnd(Controller, Target);
	bSwingMode = false;
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

void AHarpoonGun::Reload()
{
	// Do not want base reload
	// Super::Reload();
	
	CurrentHarpoon->ReturnToPlayer();
}

void AHarpoonGun::Tick(float DeltaSeconds)
{
	// Do not want base auto reload
	//Super::Tick(DeltaSeconds);
}
