// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

UCLASS()
class MI498_UEPROJECT_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	/// Sets default values for this character's properties
	ACharacterBase();
	/// Max health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health")
	float MaxHealth = 100.f;
	/// Current Health 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
	float CurrentHealth;
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,AActor* DamageCauser) override;
protected:
	/// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
