#include "PlayerAnimation.h"
#include "../Weapons/WeaponTypes.h"

void UPlayerAnimation::SetCurrentWeapon(EWeaponType Weapon)
{
	switch (Weapon)
	{
	case EWeaponType::Blunderbuss:
		BlunderbussBoneScale = FVector(1.0f);
		SwordBoneScale = FVector(0);
		HarpoonGunBoneScale = FVector(0);
		PrevWeapon = CurrentWeapon;
		CurrentWeapon = EWeaponType::Blunderbuss;
		break;
	case EWeaponType::Sword:
		BlunderbussBoneScale = FVector(0);
		SwordBoneScale = FVector(1.0f);
		HarpoonGunBoneScale = FVector(0);
		PrevWeapon = CurrentWeapon;
		CurrentWeapon = EWeaponType::Sword;
		break;
	case EWeaponType::HarpoonGun:
		BlunderbussBoneScale = FVector(0);
		SwordBoneScale = FVector(0);
		HarpoonGunBoneScale = FVector(1.0f);
		PrevWeapon = CurrentWeapon;
		CurrentWeapon = EWeaponType::HarpoonGun;
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
