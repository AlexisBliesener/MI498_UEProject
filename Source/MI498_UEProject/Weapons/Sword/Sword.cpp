#include "Sword.h"

#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"

void ASword::PrimaryAttack(AController* Controller, AActor* Target)
{
	Super::PrimaryAttack(Controller);
	
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
	
	/// Perform a hitscan trace from the camera forward
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, cameraLocation,endLocation, ECC_Pawn, TraceParams);
	
	/// Draw a debug line showing the trace in the world
	DrawDebugLine(
	GetWorld(),
	cameraLocation,
	bHit ? hitResult.ImpactPoint : endLocation,
	FColor::Red,
	false,
	1.f,
	0,
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
}

void ASword::SecondaryAttack(AController* Controller,AActor* Target)
{
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
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
	}
}