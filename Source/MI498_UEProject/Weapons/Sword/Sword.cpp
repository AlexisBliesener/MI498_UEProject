#include "Sword.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "MI498_UEProject/Interactables/ExplodingBarrel.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"

ASword::ASword()
{
	WeaponType = EWeaponType::Sword;
	ComboResetTime = ReloadTime + 0.2f;
	CurrentAmmo = 1;
}

void ASword::PrimaryAttack(AController* Controller, AActor* Target)
{
	/// No functionality
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

	Super::SecondaryAttack(Controller, Target);

	/// Get the player camera location and rotation for dash direction
	FVector cameraLocation;
	FRotator cameraRotation;
	playerController->GetPlayerViewPoint(cameraLocation, cameraRotation);
	
	/// Dashes the player forward in look direction
	playerCharacter->LaunchCharacter(cameraRotation.Vector() * DashForce, true, true);

	/// Turn off gravity during dash
	playerMovementComponent->GravityScale = 0.0f;
	GetWorld()->GetTimerManager().ClearTimer(SwordDashGravityTimerHandler);
	GetWorld()->GetTimerManager().SetTimer(
		SwordDashGravityTimerHandler,
		[this]()
		{
			if (playerCharacter && playerMovementComponent)
			{
				playerMovementComponent->GravityScale = 1.f;
			}
		},
		DashGravityOffTime,
		false
	);

	// Add invincibility 
	playerCharacter->AddInvincibility(DashInvincibilitySeconds);
	
	// Deal Damage
	bSwordDashHitboxActive = true;
	SwordDashHitboxStartTime = GetWorld()->GetTimeSeconds();
	DashHitActors.Empty();
}

void ASword::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Reload if necessary
	if (CurrentDashCharges != DashCharges && !bReloadingSecondary)
	{
		if (playerMovementComponent->IsMovingOnGround())
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

	/// Activate dash hitbox if necessary
	if (bSwordDashHitboxActive)
	{
		if (GetWorld()->GetTimeSeconds() - SwordDashHitboxStartTime > SwordDashHitboxDuration)
		{
			bSwordDashHitboxActive = false;
			return;
		}

		DashHitbox();
	}
}

void ASword::BeginPlay()
{
	Super::BeginPlay();

	bFirstAttackInSequence = true;
	
	/// Cache player components
	playerCharacter = Cast<APlayerCharacter>(GetOwner());
	playerMovementComponent = playerCharacter->GetCharacterMovement();
	playerController = Cast<APlayerController>(playerCharacter->GetInstigatorController());
	Camera = playerCharacter->FindComponentByClass<UCameraComponent>();
}

void ASword::DashHitbox()
{
	/// Array to store all hit results from the sweep
	TArray<FHitResult> hitResults;

	/// Ignore player and self collision
	FCollisionQueryParams traceParams;
	traceParams.AddIgnoredActor(this);
	traceParams.AddIgnoredActor(GetOwner());

	/// Get the current viewpoint of the player (camera position and direction)
	FVector cameraLocation;
	FRotator cameraRotation;
	playerController->GetPlayerViewPoint(cameraLocation, cameraRotation);

	/// Calculate start and end positions of the hitbox
	FVector start = cameraLocation;
	FVector end = start + cameraRotation.Vector() * 200.f;

	/// Perform a box sweep from start to end to detect actors in the dash path
	GetWorld()->SweepMultiByChannel(
		hitResults,
		start,
		end,
		cameraRotation.Quaternion(),
		ECC_Visibility,
		FCollisionShape::MakeBox(DashHalfSize),
		traceParams);

	/// Loop through every actor hit by the sweep
	for (const FHitResult& hit : hitResults)
	{
		AActor* actor = hit.GetActor();

		/// Skip if the actor is invalid or has already been hit during this dash
		if (!actor || DashHitActors.Contains(actor))
		{
			continue;
		}

		/// Add actor to the list so it can't be hit again during the same dash
		DashHitActors.Add(actor);

		/// If the actor is an exploding barrel, trigger its explosion
		if (AExplodingBarrel* barrel = Cast<AExplodingBarrel>(actor))
		{
			barrel->Explode();
		}

		/// Calculate Knockback Direction
		FVector KnockbackDir = actor->GetActorLocation() - playerCharacter->GetActorLocation();
		if (KnockbackDir.Z < 0)
		{
			KnockbackDir.Z = 0;
		}
		KnockbackDir.Normalize();

		/// Apply knockback to Character
		if (AEnemyBase* HitEnemy = Cast<AEnemyBase>(actor))
		{
			if (AController* SolCon = HitEnemy->GetController())
			{
				SolCon->StopMovement();
			}
			// Apply the physical launch
			HitEnemy->LaunchCharacter(
				(KnockbackDir * EnemyKnockbackForce.X) + FVector::UpVector * EnemyKnockbackForce.Y, true, true);
		}

		/// Apply damage to the actor that was hit
		UGameplayStatics::ApplyDamage(
			actor,
			DashDamage,
			playerController,
			this,
			nullptr
		);
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

	bool cacheSwingDirection = bFirstAttackInSequence;
	bFirstAttackInSequence = !bFirstAttackInSequence;

	/// Get the player camera location and rotation for aiming
	FVector cameraLocation;
	FRotator cameraRotation;
	Controller->GetPlayerViewPoint(cameraLocation, cameraRotation);

	if (SlashVFX)
	{
		/// Offset where the slash effect appears relative to the camera
		FVector spawnOffset = FVector(50.f, 0.f, -20.f);

		/// Scale of the slash visual effect
		FVector spawnScale = FVector(0.5f, 0.5f, 1.f);

		/// Rotation offset used to orient the slash effect
		FRotator slashOffset;

		/// Flip the slash direction depending on swing order
		if (cacheSwingDirection)
		{
			slashOffset = FRotator(-180.f, 0.f, 30.f);
		}
		else
		{
			slashOffset = FRotator(0.f, 180.f, -30.f);
		}

		/// Spawn the slash Niagara system attached to the camera
		UNiagaraComponent* NiagaraComp =
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				SlashVFX,
				Camera,
				NAME_None,
				spawnOffset,
				slashOffset,
				EAttachLocation::KeepRelativeOffset,
				true
			);

		/// Apply scale to the Niagara effect 
		if (NiagaraComp)
		{
			NiagaraComp->SetRelativeScale3D(spawnScale);
		}
	}

	/// Prepare a hit result to store the outcome of the line trace
	TArray<FHitResult> hitResults;

	/// Setup collision parameters for the trace
	FCollisionQueryParams traceParams;
	traceParams.AddIgnoredActor(this);
	traceParams.AddIgnoredActor(GetOwner());

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
				FCollisionShape::MakeBox(SwingHalfSize),
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
}

void ASword::ReloadDashes()
{
	OnReloadDashes();
	
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
