#include "PlayerCharacter.h"

#include "WeaponManager.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerCharacter::APlayerCharacter()
{
	// Weapons Manager
	WeaponManager = CreateDefaultSubobject<UWeaponManager>(TEXT("Weapons Manger"));

}

void APlayerCharacter::ToggleSprint()
{
	bIsSprinting = !bIsSprinting;
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? MaxSprintSpeed : MaxWalkSpeed;
}
