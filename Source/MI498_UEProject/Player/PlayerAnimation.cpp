#include "PlayerAnimation.h"

void UPlayerAnimation::SetCurrentWeapon(EWeaponType Weapon)
{
	switch (Weapon)
	{
	case EWeaponType::Blunderbuss:
		BlunderbussBoneScale = FVector(1.0f);
		SwordBoneScale = FVector(0);
		HarpoonGunBoneScale = FVector(0);
		break;

	case EWeaponType::Sword:
		BlunderbussBoneScale = FVector(0);
		SwordBoneScale = FVector(1.0f);
		HarpoonGunBoneScale = FVector(0);
		break;
	case EWeaponType::HarpoonGun:
		BlunderbussBoneScale = FVector(0);
		SwordBoneScale = FVector(0);
		HarpoonGunBoneScale = FVector(1.0f);
		break;
	}
}
