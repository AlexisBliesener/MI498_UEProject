#include "PlayerCharacter.h"

#include "WeaponManager.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerCharacter::APlayerCharacter()
{
	WeaponManager = CreateDefaultSubobject<UWeaponManager>(TEXT("Weapons Manger"));
}

void APlayerCharacter::ToggleSprint()
{
	bIsSprinting = !bIsSprinting;
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? MaxSprintSpeed : MaxWalkSpeed;
}
