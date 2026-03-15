#include "Blunderbuss.h"
#include "../../Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "MI498_UEProject/Interactables/ExplodingBarrel.h"
#include "NiagaraFunctionLibrary.h"

ABlunderbuss::ABlunderbuss()
{
	PrimaryActorTick.bCanEverTick = true;
	WeaponType = EWeaponType::Blunderbuss;
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
	Fire(Controller, Target, Damage, EnemyKnockbackForcePrimary);

	// Consume ammo required for a primary shot
	CurrentAmmo -= PrimaryAttackNeededAmmo;
	// Update HUD
	OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo, false);

	// If the firing controller is a player, apply recoil knockback
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		PlayerKnockback(playerController, PrimaryAttackKnockbackForce);
		ApplyCameraRecoil(playerController, true);
	}
}

void ABlunderbuss::PrimaryAttackHold(AController* Controller, AActor* Target)
{
	// No functionality
}

void ABlunderbuss::SecondaryAttack(AController* Controller, AActor* Target)
{
	// Check if there is enough ammo to perform the secondary attack
	if (CurrentAmmo - SecondaryAttackNeededAmmo < 0)
	{
		return;
	}

	Super::SecondaryAttack(Controller, Target);

	// Fire using multiplied damage for the double-shot behavior
	Fire(Controller, Target, Damage * DoubleShotDamageMultiplier, EnemyKnockbackForceSecondary);

	// Consume ammo required for a secondary shot
	CurrentAmmo -= SecondaryAttackNeededAmmo;
	// Update HUD
	OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo, true);

	// If the firing controller is a player, apply recoil knockback
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		PlayerKnockback(playerController, SecondaryAttackKnockbackForce);
		ApplyCameraRecoil(playerController, false);
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

	/// Apply physical recoil to the player if airborne
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!playerCharacter->GetCharacterMovement()->IsMovingOnGround())
	{
		/// Launch the player backward based on knockback force and firing direction
		playerCharacter->GetCharacterMovement()->AddImpulse(-cameraForwardVector * KnockbackForce, true);
	}
}

void ABlunderbuss::ApplyCameraRecoil(APlayerController* PlayerController, bool Primary)
{
	/// Ensure we have a valid player controller before applying recoil
	if (!PlayerController) return;

	/// Reset recoil tracking variables at the start of each shot
	CurrentRecoilStep = 0;
	CurrentRecoilTime = 0;

	if (Primary)
	{
		/// Use the primary recoil curve if firing primary
		if (PrimaryRecoilCurve)
		{
			/// Access the internal rich curve to determine total recoil duration
			FRichCurve* richCurve = &PrimaryRecoilCurve->FloatCurve;

			if (richCurve)
			{
				/// Get the last keyframe time to determine total recoil time
				const TArray<FRichCurveKey>& Keys = richCurve->GetConstRefOfKeys();
				RecoilTime = Keys[Keys.Num() - 1].Time;
			}
		}
	}
	else
	{
		/// Use the secondary recoil curve if firing secondary
		if (SecondaryRecoilCurve)
		{
			/// Access the internal rich curve to determine total recoil duration
			FRichCurve* richCurve = &SecondaryRecoilCurve->FloatCurve;

			if (richCurve)
			{
				/// Get the last keyframe time to determine total recoil time
				const TArray<FRichCurveKey>& Keys = richCurve->GetConstRefOfKeys();
				RecoilTime = Keys[Keys.Num() - 1].Time;
			}
		}
	}

	/// Apply recoil gradually over time using a repeating timer
	GetWorld()->GetTimerManager().SetTimer(
		RecoilTimerHandle,
		FTimerDelegate::CreateLambda([this, PlayerController, Primary]()
		{
			/// Validate controller again inside timer callback
			if (!PlayerController) return;

			/// Apply pitch input based on the current recoil curve value
			if (Primary)
			{
				PlayerController->AddPitchInput(PrimaryRecoilCurve->GetFloatValue(CurrentRecoilTime));
			}
			else
			{
				PlayerController->AddPitchInput(SecondaryRecoilCurve->GetFloatValue(CurrentRecoilTime));
			}

			/// Advance recoil step and time
			CurrentRecoilStep++;
			CurrentRecoilTime += RecoilTime / RecoilSteps;

			/// After half the steps, begin reset phase
			if (CurrentRecoilStep >= RecoilSteps / 2 && !bResetRecoil)
			{
				CurrentRecoilStep = 0;
				bResetRecoil = true;
			}
			/// After completing reset phase, stop the timer
			else if (CurrentRecoilStep >= RecoilSteps / 2 && bResetRecoil)
			{
				bResetRecoil = false;
				GetWorld()->GetTimerManager().ClearTimer(RecoilTimerHandle);
			}
		}),
		/// Interval between recoil updates
		RecoilTime / RecoilSteps,
		true
	);
}

void ABlunderbuss::Fire(AController* Controller, AActor* Target, int CurrentDamage, FVector2D EnemyKnockbackForce)
{
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
	FVector halfSize = FVector(10, 10, 10);

	/// Calculate Direction Vectors from Camera Rotation
	FVector forward = cameraRotation.Vector();
	FVector right = FRotationMatrix(cameraRotation).GetUnitAxis(EAxis::Y);
	FVector up = FRotationMatrix(cameraRotation).GetUnitAxis(EAxis::Z);

	/// Track Unique Damaged Actors
	TMap<AActor*, float> damagedActors;

	float radius = 2.f;

	// Perform Multi-Slice Box Sweeps
	// Creates a 2 (vertical) x 3 (horizontal) grid of box sweeps
	for (float i = -2; i < 3; i++)
	{
		for (int j = -2; j < 3; j++)
		{
			/// Skip points outside the circle
			if ((i * i + j * j) > radius * radius)
				continue;

			/// Offset each slice relative to camera
			FVector offset = right * j * 50.f + up * i * 50.f;

			/// Start position of this slice
			FVector start = cameraLocation;

			/// End position extends forward by weapon range
			FVector end = (start + offset) + forward * Range;

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

				float jitter = 20.f;
				FVector jitterOffset(
					FMath::FRandRange(-jitter, jitter),
					FMath::FRandRange(-jitter, jitter),
					0.f
				);

				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(),
					HitVFX,
					hit.ImpactPoint + jitterOffset,
					hit.ImpactNormal.Rotation()
				);

				float hitDistance = hit.Distance;

				// If actor already hit, keep the closest hit
				if (damagedActors.Contains(hitActor))
				{
					if (hitDistance < damagedActors[hitActor])
					{
						damagedActors[hitActor] = hitDistance;
					}
				}
				else
				{
					damagedActors.Add(hitActor, hitDistance);
				}
			}
		}
	}

	/// Apply Effects to all Unique Hit Actors
	for (auto& pair : damagedActors)
	{
		AActor* hitActor = pair.Key;
		float hitDistance = pair.Value;

		if (!hitActor) continue;

		/// Exploding barrel
		if (AExplodingBarrel* barrel = Cast<AExplodingBarrel>(hitActor))
		{
			barrel->Explode();
		}

		/// Calculate damage fall off
		int hitDamage = ((Range - hitDistance) / Range) * CurrentDamage;

		/// Calculate Knockback Direction
		FVector KnockbackDir = hitActor->GetActorLocation() - GetOwner()->GetActorLocation();
		if (KnockbackDir.Z < 0)
		{
			KnockbackDir.Z = 0;
		}
		KnockbackDir.Normalize();

		/// Apply knockback to Character

		if (AEnemyBase* HitEnemy = Cast<AEnemyBase>(hitActor))
		{
			if (AController* SolCon = HitEnemy->GetController())
			{
				SolCon->StopMovement();
			}
			// Apply the physical launch
			HitEnemy->LaunchCharacter(
				(KnockbackDir * EnemyKnockbackForce.X) + FVector::UpVector * EnemyKnockbackForce.Y, true, true);
		}

		/// Apply damage
		UGameplayStatics::ApplyDamage(
			hitActor,
			hitDamage,
			Controller,
			this,
			nullptr
		);
	}
}
