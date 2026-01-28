#include "PlayerCharacter.h"

#include "VirtualShadowMapDefinitions.h"
#include "../Weapons/WeaponManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

APlayerCharacter::APlayerCharacter()
{
	WeaponManager = CreateDefaultSubobject<UWeaponManager>(TEXT("Weapons Manger"));
	CurrentHealth = MaxHealth;
}

void APlayerCharacter::ToggleSprint()
{
	bIsSprinting = !bIsSprinting;
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? MaxSprintSpeed : MaxWalkSpeed;
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,class AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageAmount <= 0.f || CurrentHealth <= 0.f)
		return 0.f;

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);

	if (GEngine)
	{

			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Red,
				FString::Printf(
					TEXT("ATTACKIINGNGG! Player Health: %.1f / %.1f"),
					CurrentHealth,
					MaxHealth
				)
			);
		}

	if (CurrentHealth <= 0.f)
	{
		// restart level for now..
		UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()), false);
	}

	return DamageAmount;
}
