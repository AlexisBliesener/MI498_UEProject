#include "Sword.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Interactables/ExplodingBarrel.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"

ASword::ASword()
{
	WeaponType = EWeaponType::Sword;
}

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

void ASword::SecondaryAttack(AController* Controller, AActor* Target)
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
		false);

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
				false);
		}
	}
}

void ASword::SwingSword(AController* Controller, AActor* Target)
{
	/// Set combo reset timer
	GetWorld()->GetTimerManager().ClearTimer(ComboResetTimer);
	GetWorld()->GetTimerManager().SetTimer(
		ComboResetTimer,
		this,
		&ASword::ResetCombo,
		ComboResetTime,
		false);

	/// Get the player camera location and rotation for aiming
	FVector cameraLocation;
	FRotator cameraRotation;
	Controller->GetPlayerViewPoint(cameraLocation, cameraRotation);

	/// Prepare a hit result to store the outcome of the line trace
	TArray<FHitResult> hitResults;
	
	/// Setup collision parameters for the trace
	FCollisionQueryParams traceParams;
	traceParams.AddIgnoredActor(this);
	traceParams.AddIgnoredActor(GetOwner());

	/// Half size of the box thats sweeps for damage
	FVector halfSize = FVector(30, 33, 50);

	/// Calculate Direction Vectors from Camera Rotation
	FVector forward = cameraRotation.Vector();
	FVector right = FRotationMatrix(cameraRotation).GetUnitAxis(EAxis::Y);
	FVector up = FRotationMatrix(cameraRotation).GetUnitAxis(EAxis::Z);

	/// Track Unique Damaged Actors
	TSet<AActor*> damagedActors;

	// Perform Multi-Slice Box Sweeps
	// Creates a 2 (vertical) x 3 (horizontal) grid of box sweeps
	for (int i = 0; i < 2; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			/// Offset each slice relative to camera
			FVector offset = right * j * 33.f + up * i * 50.f;

			/// Start position of this slice
			FVector start = cameraLocation + offset;

			/// End position extends forward by weapon range
			FVector end = start + forward * Range;

			/// Store hits for this individual slice
			TArray<FHitResult> sliceHits;

			/// Perform box sweep along the slice path
			GetWorld()->SweepMultiByChannel(
				sliceHits,
				start,
				end,
				cameraRotation.Quaternion(),
				ECC_Visibility,
				FCollisionShape::MakeBox(halfSize),
				traceParams
			);

			/// Process all hits from this slice
			for (const FHitResult& hit : sliceHits)
			{
				AActor* hitActor = hit.GetActor();
				if (!hitActor) continue;

				/// Skip actors already processed
				if (damagedActors.Contains(hitActor))
					continue;

				/// Add unique actor to damage list
				damagedActors.Add(hitActor);
			}
		}
	}
	
	/// Apply Effects to all Unique Hit Actors
	for (AActor* hitActor : damagedActors)
	{
		if (!hitActor) continue;
		
		// Exploding barrel
		if (AExplodingBarrel* barrel = Cast<AExplodingBarrel>(hitActor))
		{
			barrel->Explode();
		}

		// Apply damage
		UGameplayStatics::ApplyDamage(
			hitActor,
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
	OnAmmoChanged.Broadcast(CurrentDashCharges, DashCharges, true);
}

void ASword::ResetCombo()
{
	bFirstAttackInSequence = true;
}
