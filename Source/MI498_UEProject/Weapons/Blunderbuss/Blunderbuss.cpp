#include "Blunderbuss.h"
#include "../../Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABlunderbuss::ABlunderbuss()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABlunderbuss::PrimaryAttack(AController* Controller, AActor* Target)
{
	// Check if there is enough ammo to perform the primary attack
	if (CurrentAmmo - PrimaryAttackNeededAmmo < 0)
	{
		return;
	}
	
	Super::PrimaryAttack(Controller, Target);
	
	// Perform the actual weapon fire trace and damage calculation
	Fire(Controller, Target, Damage);
	
	// Consume ammo required for a primary shot
	CurrentAmmo -= PrimaryAttackNeededAmmo;
	
	// If the firing controller is a player, apply recoil knockback
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		PlayerKnockback(playerController, PrimaryAttackKnockbackForce);
	}
}

void ABlunderbuss::PrimaryAttackHold(AController* Controller, AActor* Target)
{
	// No functionality
}

void ABlunderbuss::SecondaryAttack(AController* Controller,AActor* Target)
{
	// Check if there is enough ammo to perform the secondary attack
	if (CurrentAmmo - SecondaryAttackNeededAmmo < 0)
	{
		return;
	}
	
	Super::SecondaryAttack(Controller, Target);
	
	// Fire using multiplied damage for the double-shot behavior
	Fire(Controller, Target, Damage * DoubleShotDamageMultiplier);
	
	// Consume ammo required for a secondary shot
	CurrentAmmo -= SecondaryAttackNeededAmmo;
	
	// If the firing controller is a player, apply recoil knockback
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		PlayerKnockback(playerController, SecondaryAttackKnockbackForce);
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

void ABlunderbuss::Fire(AController* Controller, AActor* Target, int CurrentDamage)
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
	
	/// Half size of the box thats sweeps for damage
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
	
	/// Check if HitResult hit an enemy and apply damage
	if (bHit && hitResult.GetActor())
	{
		UGameplayStatics::ApplyDamage(
			hitResult.GetActor(),
			hitDamage, 
			Controller,
			this,
			nullptr
		);
	}
}



