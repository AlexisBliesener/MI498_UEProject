#include "Blunderbuss.h"
#include "../../Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Interactables/ExplodingBarrel.h"

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
	Fire(Controller, Target, Damage);
	
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
		playerCharacter->GetCharacterMovement()->AddImpulse(-cameraForwardVector * KnockbackForce,true);
	}
}


void ABlunderbuss::ApplyCameraRecoil(APlayerController* PlayerController, bool Primary)
{
	if (!PlayerController) return;
	
	
	CurrentRecoilStep = 0;
	CurrentRecoilTime = 0;
	
	if (Primary)
	{
		if (PrimaryRecoilCurve)
		{
			// Access the internal rich curve
			FRichCurve* richCurve = &PrimaryRecoilCurve->FloatCurve;

			if (richCurve)
			{
				const TArray<FRichCurveKey>& Keys = richCurve->GetConstRefOfKeys();
				RecoilTime = Keys[Keys.Num()-1].Time;
			}
		}
	
	}
	else
	{
		if (SecondaryRecoilCurve)
		{
			// Access the internal rich curve
			FRichCurve* richCurve = &SecondaryRecoilCurve->FloatCurve;

			if (richCurve)
			{
				const TArray<FRichCurveKey>& Keys = richCurve->GetConstRefOfKeys();
				RecoilTime = Keys[Keys.Num()-1].Time;
			}
		}
	
	}
	
	// Apply recoil gradually using a timer
	GetWorld()->GetTimerManager().SetTimer(
		RecoilTimerHandle,
		FTimerDelegate::CreateLambda([this, PlayerController, Primary]()
		{
			if (!PlayerController) return;

			if (Primary)
			{
				PlayerController->AddPitchInput(PrimaryRecoilCurve->GetFloatValue(CurrentRecoilTime));	
			}
			else
			{
				PlayerController->AddPitchInput(SecondaryRecoilCurve->GetFloatValue(CurrentRecoilTime));	
			}
		

			CurrentRecoilStep++;
			CurrentRecoilTime += RecoilTime / RecoilSteps;

			if (CurrentRecoilStep >= RecoilSteps/2 && !bResetRecoil)
			{
				CurrentRecoilStep = 0;
				bResetRecoil = true;
			}
			else if (CurrentRecoilStep >= RecoilSteps/2 && bResetRecoil)
			{
				bResetRecoil = false;
				GetWorld()->GetTimerManager().ClearTimer(RecoilTimerHandle);
			}
		}),
		RecoilTime / RecoilSteps,
		true
	);
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
	ECC_Visibility,
	FCollisionShape::MakeBox(halfSize),
	TraceParams);
	
	/// Draw a debug line showing the trace in the world
	DrawDebugBox(
	GetWorld(),
	 bHit ? hitResult.Location : endLocation,
	halfSize,
	cameraRotation.Quaternion(),
	FColor::Red,
	false,
	1.f
	);
	
	/// If an exploding barrel was hit
	if (bHit)
	{
		if (AExplodingBarrel* barrel = Cast<AExplodingBarrel>(hitResult.GetActor()))
		{
			barrel->Explode();
		}
	}
	
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



