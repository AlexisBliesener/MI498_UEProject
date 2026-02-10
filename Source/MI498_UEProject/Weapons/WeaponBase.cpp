#include "WeaponBase.h"

DEFINE_LOG_CATEGORY(WeaponLog)

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeaponBase::PrimaryAttack(AController* PlayerController, AActor* Target)
{
	OnPrimaryAttack();
}

void AWeaponBase::PrimaryAttackHold(AController* Controller, AActor* Target)
{
	OnPrimaryAttack();
}

void AWeaponBase::PrimaryAttackHoldStart(AController* Controller, AActor* Target)
{
}

void AWeaponBase::PrimaryAttackHoldEnd(AController* Controller, AActor* Target)
{
}

void AWeaponBase::SecondaryAttack(AController* Controller,AActor* Target)
{
	OnSecondaryAttack();
}

void AWeaponBase::SecondaryAttackHoldStart(AController* Controller, AActor* Target)
{
}

void AWeaponBase::SecondaryAttackHoldEnd(AController* Controller, AActor* Target)
{
}

void AWeaponBase::Reload()
{
	if (bReloading) return;
	
	OnReload();
	bReloading = true;
	ReloadTimer = GetWorld()->GetTimeSeconds();
}

void AWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	/// Auto reload
	if (CurrentAmmo == 0)
	{
		Reload();
	}
	
	// If currently reloading and enough time has passed,
	// complete the reload and restore ammo
	if (bReloading && GetWorld()->GetTimeSeconds() > ReloadTimer + ReloadTime)
	{
		bReloading = false;	
		CurrentAmmo = MaxAmmo;
	}
}
