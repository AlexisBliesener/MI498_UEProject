#include "Blunderbuss.h"

#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void ABlunderbuss::Fire(APlayerController* PlayerController)
{
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	
	FHitResult HitResult;
	
	FVector CameraForwardVector = CameraRotation.Vector();
	FVector EndLocation = CameraLocation + CameraForwardVector * Range;
	
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(GetOwner());
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation,EndLocation, ECC_Visibility, TraceParams);
	
	DrawDebugLine(
	GetWorld(),
	CameraLocation,
	bHit ? HitResult.ImpactPoint : EndLocation,
	FColor::Red,
	false,
	1.f,
	0,
	1.f
	);
	
	// CHECK IF HITRESULT IS ENEMY IF SO DO DAMAGE
	
	// Camera recoil
	PlayerController->AddPitchInput(CameraRecoil);
	
	// Recoil
	// check for player is not on ground
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!PlayerCharacter->GetCharacterMovement()->IsMovingOnGround())
	{
		FVector LaunchVelocity = -CameraForwardVector * KnockbackForce;
		
		PlayerCharacter->LaunchCharacter(LaunchVelocity, false, false);
		GEngine->AddOnScreenDebugMessage(
					-1,                // key (-1 = new message every time)
					2.f,               // duration in seconds
					FColor::Green,     // text color
					TEXT("Player is not on the ground")
				);
	}
}

