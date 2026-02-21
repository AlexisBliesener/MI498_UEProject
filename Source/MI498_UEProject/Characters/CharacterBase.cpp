// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"


// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

float ACharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageAmount <= 0.f || CurrentHealth <= 0.f || bIsInvincible)
		return 0.f;

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
	// Send an event to OnDamage 
	OnDamage.Broadcast();
	
	return DamageAmount;
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

void ACharacterBase::HealCharacter(float HealAmount)
{
	if (HealAmount <= 0.f || CurrentHealth <= 0.f || CurrentHealth >= MaxHealth)
		return;

	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.f, MaxHealth);
}

