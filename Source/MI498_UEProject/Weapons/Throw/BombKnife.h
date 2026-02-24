// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MI498_UEProject/Weapons/WeaponBase.h"
#include "BombKnife.generated.h"

class ABombProjectile;

UCLASS()
class MI498_UEPROJECT_API ABombKnife : public AWeaponBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABombKnife();
	
	/// Launch speed multiplier for arc
	UPROPERTY(NotBlueprintable)
	float LaunchSpeed = 1200.f;
	/// Second Attack damage for this weapon 
	UPROPERTY(NotBlueprintable)
	float SecondAttackDamage = 2.f;
	/// Second Attack radius
	UPROPERTY(NotBlueprintable)
	float SecondAttackRadius = 200.f;
	UPROPERTY(NotBlueprintable)
	float ExplosionRadius = 300.f;
protected:
	virtual void PrimaryAttack(AController* Controller, AActor* Target = nullptr) override;

	virtual void SecondaryAttack(AController* Controller, AActor* Target = nullptr) override;

private:
	/// Projectile class for the bomb
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<ABombProjectile> BombProjectileClass;
	/// Socket name for the pistol (we need this when the enemy character is ready, so it can shoot from the hand) 
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName SocketName;
};
