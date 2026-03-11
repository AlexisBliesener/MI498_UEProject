// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamage);

UCLASS()
class MI498_UEPROJECT_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	/// Sets default values for this character's properties
	ACharacterBase();
	ACharacterBase(const FObjectInitializer& ObjectInitializer);
	/// Max health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Health")
	float MaxHealth = 100.f;
	/// Current Health 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default|Health")
	float CurrentHealth;
	/// Damage event (event dispatcher)
	UPROPERTY(BlueprintAssignable, Category="Health", meta = (ToolTip="Fires when the character is damaged"))
	FOnDamage OnDamage;
	virtual void Die() {};
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,AActor* DamageCauser) override;
protected:
	/// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/// Whether the player is currently invincible
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Health")
	bool bIsInvincible = false;

	// Give health to the character
	UFUNCTION(BlueprintCallable, Category = "Health")
	void HealCharacter(float HealAmount);
};
