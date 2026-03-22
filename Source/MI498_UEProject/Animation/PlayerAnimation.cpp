#include "PlayerAnimation.h"
#include "../Weapons/WeaponTypes.h"

void UPlayerAnimation::SetCurrentWeapon(EWeaponType Weapon)
{
	PrevWeapon = CurrentWeapon;
	CurrentWeapon = Weapon;
	
	GetWorld()->GetTimerManager().ClearTimer(WeaponScaleTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(
		WeaponScaleTimerHandle,
		FTimerDelegate::CreateUObject(this, &UPlayerAnimation::ApplyWeaponScale, Weapon),
		0.2f,
		false
	);
}

void UPlayerAnimation::ApplyWeaponScale(EWeaponType Weapon)
{
	// Reset all
	BlunderbussBoneScale = FVector::ZeroVector;
	SwordBoneScale = FVector::ZeroVector;
	HarpoonGunBoneScale = FVector::ZeroVector;

	// Activate the correct one
	switch (Weapon)
	{
	case EWeaponType::Blunderbuss:
		BlunderbussBoneScale = FVector(1.0f);
		break;

	case EWeaponType::Sword:
		SwordBoneScale = FVector(1.0f);
		break;

	case EWeaponType::HarpoonGun:
		HarpoonGunBoneScale = FVector(1.0f);
		break;
	}
}

EWeaponTransitionMontageToPlay UPlayerAnimation::GetTransitionMontageToPlay()
{
	switch (CurrentWeapon)
	{
	case EWeaponType::Blunderbuss:
		switch (PrevWeapon)
		{
		case EWeaponType::Sword:
			return EWeaponTransitionMontageToPlay::SwordToBlunderbuss;
		case EWeaponType::HarpoonGun:
			return EWeaponTransitionMontageToPlay::HarpoonGunToBlunderbuss;
		}
		break;
	case EWeaponType::Sword:
		switch (PrevWeapon)
		{
		case EWeaponType::Blunderbuss:
			return EWeaponTransitionMontageToPlay::BlunderbussToSword;
		case EWeaponType::HarpoonGun:
			return EWeaponTransitionMontageToPlay::HarpoonGunToSword;
		}
		break;
	case EWeaponType::HarpoonGun:
		switch (PrevWeapon)
		{
		case EWeaponType::Blunderbuss:
			return EWeaponTransitionMontageToPlay::BlunderbussToHarpoonGun;
		case EWeaponType::Sword:
			return EWeaponTransitionMontageToPlay::SwordToHarpoonGun;
		}
		break;
	}

	return EWeaponTransitionMontageToPlay::None;
}
