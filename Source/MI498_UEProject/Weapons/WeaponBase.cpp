#include "WeaponBase.h"

DEFINE_LOG_CATEGORY(WeaponLog)

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeaponBase::PrimaryAttack(AController* PlayerController, AActor* Target)
{
	// Triggers the attack event hook (animation/SFX/VFX)
	OnPrimaryAttack();
}

void AWeaponBase::SecondaryAttack(AController* Controller,AActor* Target)
{
}

void AWeaponBase::Reload()
{
	bReloading = true;
	ReloadTimer = GetWorld()->GetTimeSeconds();
}

void AWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// If currently reloading and enough time has passed,
	// complete the reload and restore ammo
	if (bReloading && GetWorld()->GetTimeSeconds() > ReloadTimer + ReloadTime)
	{
		bReloading = false;	
		CurrentAmmo = MaxAmmo;
	}
}
