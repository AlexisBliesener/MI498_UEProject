// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"
#include "MI498_UEProject/Weapons/Pistol/PistolProjectile.h"
#include "EnemyBase.generated.h"

UCLASS(Blueprintable, meta=(AllowDerivedTypes="true"))
class MI498_UEPROJECT_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Combat")
	float AttackRange = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Combat")
	float ShootCooldown = 1.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Movement")
	float ChaseSpeed = 450.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Health")
	float MaxHealth = 100.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|Health")
	float CurrentHealth;
	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	void ShootPistol();
	AEnemyBase();
	virtual void Tick(float DeltaTime) override;
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	UFUNCTION(BlueprintPure, Category = "Player|AI", meta = (DisplayName = "Get State Tree", ReturnDisplayName = "State Tree"))
	UStateTree* GetStateTree() const;
	
	UFUNCTION(BlueprintCallable, Category="Enemy|Combat")
	void Attack(AActor* Target);
	UPROPERTY(EditDefaultsOnly, Category="Enemy|Combat")
	TSubclassOf<APistolProjectile> ProjectileClass;
	
protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,AActor* DamageCauser) override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|AI", meta = (AllowPrivateAccess = "true"))
	UStateTree* StateTreesss;
	bool bCanShoot = true;
	FTimerHandle ShootTimer;

	void ResetShoot();
};
