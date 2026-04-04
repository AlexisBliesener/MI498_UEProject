#include "Blunderbuss.h"

#include "NiagaraComponent.h"
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
	// Return if currently reloading 
	if (bReloading) return;

	// Check if there is enough ammo to perform the primary attack
	if (CurrentAmmo - PrimaryAttackNeededAmmo < 0)
	{
		OnFiredWithNotEnoughAmmo();
		return;
	}

	Super::PrimaryAttack(Controller, Target);

	// Perform the actual weapon fire trace and damage calculation
	Fire(Controller, Target, Damage, EnemyKnockbackForcePrimary);

	// Consume ammo required for a primary shot
	CurrentAmmo -= PrimaryAttackNeededAmmo;

	/// Sets reload time based on ammo left
	ReloadTime = CurrentAmmo == 1 ? OneAmmoReloadTime : TwoAmmoReloadTime;

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
	// Return if currently reloading 
	if (bReloading) return;

	// Check if there is enough ammo to perform the secondary attack
	if (CurrentAmmo - SecondaryAttackNeededAmmo < 0)
	{
		OnFiredWithNotEnoughAmmo();
		return;
	}

	Super::SecondaryAttack(Controller, Target);

	// Fire using multiplied damage for the double-shot behavior
	Fire(Controller, Target, Damage * DoubleShotDamageMultiplier, EnemyKnockbackForceSecondary);

	// Consume ammo required for a secondary shot
	CurrentAmmo -= SecondaryAttackNeededAmmo;

	/// Sets reload time based on ammo left
	ReloadTime = TwoAmmoReloadTime;

	// Update HUD
	OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo, true);

	// If the firing controller is a player, apply recoil knockback
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		PlayerKnockback(playerController, SecondaryAttackKnockbackForce);
		ApplyCameraRecoil(playerController, false);
	}
}

void ABlunderbuss::BeginPlay()
{
	Super::BeginPlay();
	OneAmmoReloadTime = ReloadTime;

	// Cache player mesh
	ACharacter* PlayerCharacter = Cast<ACharacter>(GetOwner());
	USkeletalMeshComponent* MeshComp = PlayerCharacter->GetMesh();
	PlayerMesh = MeshComp;
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

struct FRecoilInstance
{
	int Step = 0;
	float Time = 0.f;
	bool bReset = false;
	FTimerHandle Handle;
};

void ABlunderbuss::ApplyCameraRecoil(APlayerController* PlayerController, bool Primary)
{
	if (!IsValid(PlayerController)) return;

	FRecoilInstance* Recoil = new FRecoilInstance();
	Recoil->Step = 0;
	Recoil->Time = 0.f;
	Recoil->bReset = false;
	float LocalRecoilTime = 0.f;

	if (Primary && PrimaryRecoilCurve)
	{
		const TArray<FRichCurveKey>& Keys = PrimaryRecoilCurve->FloatCurve.GetConstRefOfKeys();
		LocalRecoilTime = Keys.Last().Time;
	}
	else if (!Primary && SecondaryRecoilCurve)
	{
		const TArray<FRichCurveKey>& Keys = SecondaryRecoilCurve->FloatCurve.GetConstRefOfKeys();
		LocalRecoilTime = Keys.Last().Time;
	}

	FTimerDelegate RecoilDelegate = FTimerDelegate::CreateLambda(
		[this, PlayerController, Primary, Recoil, LocalRecoilTime]()
		{
			if (!PlayerController || !Recoil) return;

			float Value = 0.f;

			if (Primary && PrimaryRecoilCurve)
			{
				Value = PrimaryRecoilCurve->GetFloatValue(Recoil->Time);
			}
			else if (!Primary && SecondaryRecoilCurve)
			{
				Value = SecondaryRecoilCurve->GetFloatValue(Recoil->Time);
			}

			PlayerController->AddPitchInput(Value);

			Recoil->Step++;
			Recoil->Time += LocalRecoilTime / RecoilSteps;

			if (Recoil->Step >= RecoilSteps / 2 && !Recoil->bReset)
			{
				Recoil->Step = 0;
				Recoil->bReset = true;
			}
			else if (Recoil->Step >= RecoilSteps / 2 && Recoil->bReset)
			{
				GetWorld()->GetTimerManager().ClearTimer(Recoil->Handle);
				delete Recoil;
			}
		}
	);

	GetWorld()->GetTimerManager().SetTimer(
		Recoil->Handle,
		RecoilDelegate,
		LocalRecoilTime / RecoilSteps,
		true
	);
}

void ABlunderbuss::Fire(AController* Controller, AActor* Target, int CurrentDamage, FVector2D EnemyKnockbackForce)
{
	/// Get the camera position and rotation so the weapon fires where the player is aiming
	FVector cameraLocation;
	FRotator cameraRotation;
	Controller->GetPlayerViewPoint(cameraLocation, cameraRotation);

	/// Stores all hit results from the sweeps
	TArray<FHitResult> hitResults;

	/// Setup collision query parameters and ignore the weapon and its owner
	FCollisionQueryParams traceParams;
	traceParams.AddIgnoredActor(this);
	traceParams.AddIgnoredActor(GetOwner());

	/// Size of the box used for each sweep trace
	FVector halfSize = FVector(10, 10, 10);

	/// Create forward, right, and up vectors from the camera rotation
	FVector forward = cameraRotation.Vector();
	FVector right = FRotationMatrix(cameraRotation).GetUnitAxis(EAxis::Y);
	FVector up = FRotationMatrix(cameraRotation).GetUnitAxis(EAxis::Z);

	/// Track actors hit and the closest hit distance for each
	TMap<AActor*, float> damagedActors;

	/// Perform multiple box sweeps arranged in a circular grid pattern
	for (float i = -2; i < 3; i++)
	{
		for (int j = -2; j < 3; j++)
		{
			/// Skip grid positions outside the circular spread
			if ((i * i + j * j) > 2.f * 2.f)
				continue;

			/// Offset each sweep to simulate shotgun pellet spread
			FVector offset = right * j * 50.f + up * i * 50.f;

			/// Starting point of the sweep
			FVector start = cameraLocation;

			/// End point extends forward based on weapon range
			FVector end = (start + offset) + forward * Range;

			/// Store hits detected by this specific sweep
			TArray<FHitResult> sliceHits;

			/// Sweep a box from start to end to detect hit actors
			GetWorld()->SweepMultiByChannel(
				sliceHits,
				start,
				end,
				cameraRotation.Quaternion(),
				ECC_Visibility,
				FCollisionShape::MakeBox(halfSize),
				traceParams
			);

			/// Create tracer line vfx
			if (TracerVFX)
			{
				/// Set end point of tracer line
				FVector tracerEnd = end;
				if (sliceHits.Num() > 0)
				{
					tracerEnd = sliceHits[0].ImpactPoint;
				}

				/// Spawn tracer line attached to blunderbuss socket
				UNiagaraComponent* tracer = UNiagaraFunctionLibrary::SpawnSystemAttached(
					TracerVFX,
					PlayerMesh,
					TEXT("BlunderBussBaseSocket"),
					FVector::ZeroVector,
					FRotator::ZeroRotator,
					EAttachLocation::SnapToTarget,
					true
				);

				/// Offset start location into circular grid
				if (tracer)
				{
					const FTransform socketTransform = PlayerMesh->GetSocketTransform(TEXT("BlunderBussBaseSocket"));
					FVector startOffset = socketTransform.GetUnitAxis(EAxis::X) * -i * 1.5f + socketTransform.
						GetUnitAxis(EAxis::Z) * j * 1.5f;
					tracer->SetVectorParameter(TEXT("Start"), socketTransform.GetLocation() + startOffset);
					tracer->SetVectorParameter(TEXT("End"), tracerEnd);
				}
			}

			/// Process every hit detected in this sweep
			for (const FHitResult& hit : sliceHits)
			{
				AActor* hitActor = hit.GetActor();
				if (!hitActor) continue;

				/// Add a small random offset so impact VFX are less uniform
				FVector jitterOffset(
					FMath::FRandRange(-20.f, 20.f),
					FMath::FRandRange(-20.f, 20.f),
					0.f
				);

				/// Spawn impact vfx
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(),
					HitVFX,
					hit.ImpactPoint + jitterOffset,
					hit.ImpactNormal.Rotation()
				);

				float hitDistance = hit.Distance;

				/// Track only the closest hit for each actor
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

	/// Apply damage and effects to all unique actors that were hit
	for (auto& pair : damagedActors)
	{
		AActor* hitActor = pair.Key;
		float hitDistance = pair.Value;

		if (!hitActor) continue;

		/// Trigger explosion if the actor is an explosive barrel
		if (AExplodingBarrel* barrel = Cast<AExplodingBarrel>(hitActor))
		{
			barrel->Explode();
		}

		/// Calculate damage falloff based on distance from the player
		int hitDamage = ((Range - hitDistance) / Range) * CurrentDamage;

		/// Determine knockback direction from player to hit actor
		FVector KnockbackDir = hitActor->GetActorLocation() - GetOwner()->GetActorLocation();
		if (KnockbackDir.Z < 0)
		{
			KnockbackDir.Z = 0;
		}
		KnockbackDir.Normalize();

		/// Apply knockback if the hit actor is an enemy
		if (AEnemyBase* HitEnemy = Cast<AEnemyBase>(hitActor))
		{
			/// Stop AI movement before applying launch force
			if (AController* SolCon = HitEnemy->GetController())
			{
				SolCon->StopMovement();
			}

			/// Launch enemy using horizontal and vertical knockback forces
			HitEnemy->LaunchCharacter(
				(KnockbackDir * EnemyKnockbackForce.X) + FVector::UpVector * EnemyKnockbackForce.Y, true, true);
		}

		/// Apply damage to the actor
		UGameplayStatics::ApplyDamage(
			hitActor,
			hitDamage,
			Controller,
			this,
			nullptr
		);
	}
}
