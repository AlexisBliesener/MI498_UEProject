#include "Blunderbuss.h"
#include "../../Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABlunderbuss::ABlunderbuss()
{
	PrimaryActorTick.bCanEverTick = true;
}

/// Implements the Blunderbuss' primary attack
/// This function handles hitscan firing, camera recoil, and knockback
/// @param Controller - The player who is firing the weapon
/// @param Target The optional target actor for the attack. This is usually used by the enemy but it can be used for the player too 
void ABlunderbuss::PrimaryAttack(AController* Controller, AActor* Target)
{
	if (CurrentAmmo - 1 < 0)
	{
		return;
	}
	
	OnPrimaryAttack();
	
	Fire(Controller, Target, Damage);
	
	CurrentAmmo--;
	
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		PlayerKnockback(playerController, PrimaryAttackKnockbackForce);
	}
}

void ABlunderbuss::SecondaryAttack(AController* Controller,AActor* Target)
{
	if (CurrentAmmo - 2 < 0)
	{
		return;
	}
	
	Fire(Controller, Target, Damage * DoubleShotDamageMultiplier);
	
	CurrentAmmo -= 2;
	
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		PlayerKnockback(playerController, SecondaryAttackKnockbackForce);
	}
}

void ABlunderbuss::Reload()
{
	if (bReloading) return;
	Super::Reload();
}

void ABlunderbuss::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (CurrentAmmo == 0)
	{
		Reload();
	}
}

void ABlunderbuss::PlayerKnockback(APlayerController* PlayerController, int KnockbackForce) const
{
	/// Get the player camera location and rotation for aiming
	FVector cameraLocation;
	FRotator cameraRotation;
	PlayerController->GetPlayerViewPoint(cameraLocation, cameraRotation);
	
	/// Calculate the end location of the trace based on weapon range
	FVector cameraForwardVector = cameraRotation.Vector();
	
	/// Apply camera recoil to simulate weapon kickback
	PlayerController->AddPitchInput(CameraRecoil);
	
	/// Apply physical recoil to the player if airborne
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!playerCharacter->GetCharacterMovement()->IsMovingOnGround())
	{
		/// Launch the player backward based on knockback force and firing direction
		FVector launchVelocity = -cameraForwardVector * KnockbackForce;
		playerCharacter->LaunchCharacter(launchVelocity, false, false);
	}
}

void ABlunderbuss::Fire(AController* Controller, AActor* Target, int CurrentDamage) const
{
	/// Get the player camera location and rotation for aiming
	FVector cameraLocation;
	FRotator cameraRotation;
	Controller->GetPlayerViewPoint(cameraLocation, cameraRotation);
	
	/// Prepare a hit result to store the outcome of the line trace
	FHitResult hitResult;
	
	/// Calculate the end location of the trace based on weapon range
	FVector cameraForwardVector = cameraRotation.Vector();
	FVector endLocation = cameraLocation + cameraForwardVector * Range;
	
	/// Setup collision parameters for the trace
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(GetOwner());
	
	FVector halfSize = FVector(10, 50.f, 50); 
	
	/// Perform a hitscan trace from the camera forward
	bool bHit = GetWorld()->SweepSingleByChannel(
	hitResult,
	cameraLocation,
	endLocation,
	cameraRotation.Quaternion(),
	ECC_Pawn,
	FCollisionShape::MakeBox(halfSize),
	TraceParams
	);
	
	
	/// Draw a debug line showing the trace in the world
	DrawDebugBox(
	GetWorld(),
	 bHit ? hitResult.ImpactPoint : endLocation,
	halfSize,
	cameraRotation.Quaternion(),
	FColor::Red,
	false,
	1.f
	);
	
	//Calculate damage fall off
	int hitDamage = ((Range - hitResult.Distance)/Range) * CurrentDamage;
	UE_LOG(LogTemp, Warning, TEXT("hitDamage: %d"), hitDamage);
	
	/// Check if HitResult hit an enemy and apply damage
	if (bHit && hitResult.GetActor())
	{
		UGameplayStatics::ApplyDamage(
			hitResult.GetActor(),
			hitDamage, // weapon damage
			Controller,
			GetOwner(),
			nullptr
		);
	}
}



