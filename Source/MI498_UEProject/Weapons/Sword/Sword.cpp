#include "Sword.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"

void ASword::PrimaryAttack(AController* Controller, AActor* Target)
{
	/// Check if there is enough ammo to perform the primary attack
	if (CurrentAmmo - PrimaryAttackNeededAmmo < 0)
	{
		return;
	}
	CurrentAmmo -= PrimaryAttackNeededAmmo;
	
	Super::PrimaryAttack(Controller);
	
	SwingSword(Controller, Target);
}

void ASword::PrimaryAttackHold(AController* Controller, AActor* Target)
{
	// Check if there is enough ammo to perform the primary attack
	if (CurrentAmmo - PrimaryAttackNeededAmmo < 0)
	{
		return;
	}
	CurrentAmmo -= PrimaryAttackNeededAmmo;
	
	Super::PrimaryAttackHold(Controller, Target);
	
	SwingSword(Controller, Target);
}

void ASword::SecondaryAttack(AController* Controller,AActor* Target)
{
	// Reduce charges
	if (CurrentDashCharges == 0 || !bCanUseSecondary)
	{
		return;
	}
	CurrentDashCharges--;
	// Update HUD
	OnAmmoChanged.Broadcast(CurrentDashCharges, DashCharges, true);
	
	// Wait for cooldown to use again
	SetCanUseSecondary(false);
	FTimerDelegate delegate;
	delegate.BindUFunction(this, FName("SetCanUseSecondary"), true);
	GetWorld()->GetTimerManager().SetTimer(
		SecondaryCooldownTimerHandle,
		delegate,
		SecondaryCooldownTime,   
		false ); 
	
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		Super::SecondaryAttack(Controller, Target);
		
		/// Get the player camera location and rotation for dash direction
		FVector cameraLocation;
		FRotator cameraRotation;
		playerController->GetPlayerViewPoint(cameraLocation, cameraRotation);
		FVector cameraForwardVector = cameraRotation.Vector();
	
		/// Get a reference to the owning player character
		APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner());
	
		/// Dashes the player forward in look direction
		FVector launchVelocity = cameraForwardVector * DashForce;
		playerCharacter->LaunchCharacter(launchVelocity, true, true);
		
		// Add invincibility 
		playerCharacter->AddInvincibility(DashInvincibilitySeconds);
	}
}

void ASword::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// Reload if necessary
	if (CurrentDashCharges != DashCharges && !bReloadingSecondary)
	{
		APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (playerCharacter->GetCharacterMovement()->IsMovingOnGround())
		{
			bCanUseSecondary = false;
			bReloadingSecondary = true;
			GetWorld()->GetTimerManager().SetTimer(
				SecondaryReloadTimerHandle,
				this,
				&ASword::ReloadDashes,
				SecondaryReloadTime,   
				false ); 
		}
	}
}

void ASword::SwingSword(AController* Controller, AActor* Target)
{
	
	GetWorld()->GetTimerManager().ClearTimer(ComboResetTimer);
	
	GetWorld()->GetTimerManager().SetTimer(
	ComboResetTimer,
	this,
	&ASword::ResetCombo,
	ComboResetTime,
	false   );
	
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
	FVector halfSize = FVector(10, 70.f, 70); 
	
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
	
	/// Check if HitResult hit an enemy and apply damage
	if (bHit && hitResult.GetActor())
	{
		UGameplayStatics::ApplyDamage(
			hitResult.GetActor(),
			Damage, 
			Controller,
			this,
			nullptr
		);
	}

	bFirstAttackInSequence = !bFirstAttackInSequence;
}

void ASword::ReloadDashes()
{
	bReloadingSecondary = false;
	bCanUseSecondary = true;
	CurrentDashCharges = DashCharges;
	// Update HUD
	OnAmmoChanged.Broadcast(CurrentDashCharges,DashCharges,true);
}

void ASword::ResetCombo()
{
	bFirstAttackInSequence = true;
}
