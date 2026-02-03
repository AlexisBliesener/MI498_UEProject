#include "PlayerCharacter.h"
#include "../Weapons/WeaponManager.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	/// Create weapon manager
	WeaponManager = CreateDefaultSubobject<UWeaponManager>(TEXT("Weapons Manger"));
	
	/// Add first person camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	Camera->SetupAttachment(RootComponent);
	Camera->bUsePawnControlRotation = true;
	Camera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
}

void APlayerCharacter::ToggleSprint()
{
	bIsSprinting = !bIsSprinting;
	if (bIsSprinting)
	{
		OnSprint();
	}
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? MaxSprintSpeed : MaxWalkSpeed;
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,class AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsInvincible)
	{
		return 0;
	}
	
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (CurrentHealth <= 0.f)
	{
		// restart level for now..
		UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()), false);
	}
	return DamageAmount;
}

void APlayerCharacter::AddInvincibility(const float Seconds)
{
	InvincibilityTimer = GetWorld()->GetTimeSeconds() + Seconds;
}

void APlayerCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	bIsInvincible = InvincibilityTimer >= GetWorld()->GetTimeSeconds();
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	OnPlayerLanded();
}
