// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PistolProjectile.h"
#include "MI498_UEProject/Weapons/WeaponBase.h"
#include "Pistol.generated.h"

UCLASS()
class MI498_UEPROJECT_API APistol : public AWeaponBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APistol();

protected:
	virtual void PrimaryAttack(AController* Controller, AActor* Target = nullptr) override;

private:
	/// Projectile class for the bullet of the pistol 
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<APistolProjectile> ProjectileClass;
};
