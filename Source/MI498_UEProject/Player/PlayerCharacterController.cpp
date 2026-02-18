#include "PlayerCharacterController.h"

#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MI498_UEProject/Interactables/InteractableComponent.h"
#include "MI498_UEProject/Weapons/WeaponManager.h"

/// Defines the log category used by the player character controller
DEFINE_LOG_CATEGORY(PlayerLog);

void APlayerCharacterController::OnPossess(APawn* PossessedPawn)
{
	Super::OnPossess(PossessedPawn);
	
	/// Cache the possessed player character
	PlayerCharacter = Cast<APlayerCharacter>(PossessedPawn);
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(PlayerLog, Error, TEXT("APlayerCharacter can not be derived from the possessed pawn"));
	}
	
	/// Cache the enhanced input component for action binding
	EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComponent))
	{
		UE_LOG(PlayerLog, Error, TEXT("Unable to get reference to the EnhancedInputComponent"));
	}
	
	/// Retrieve the enhanced input subsystem for this local player
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem))
	{
		UE_LOG(PlayerLog, Error, TEXT("Unable to get reference to the UEnhancedInputLocalPlayerSubsystem"));
	}
	
	/// Reset existing input mappings and apply this controller's mapping context
	InputSubsystem->ClearAllMappings();
	InputSubsystem->AddMappingContext(InputMappingContext, 0);
	
	/// Bind inputs
	if (ActionMove)
	{
		EnhancedInputComponent->BindAction(ActionMove, ETriggerEvent::Triggered, this, &APlayerCharacterController::HandleMove);
	}
	if (ActionJump)
	{
		EnhancedInputComponent->BindAction(ActionJump, ETriggerEvent::Triggered, this, &APlayerCharacterController::HandleJump);
	}
	if (ActionLook)
	{
		EnhancedInputComponent->BindAction(ActionLook, ETriggerEvent::Triggered, this, &APlayerCharacterController::HandleLook);
	}
	if (ActionInteract)
	{
		EnhancedInputComponent->BindAction(ActionInteract, ETriggerEvent::Triggered, this, &APlayerCharacterController::HandleInteract);
	}
}

void APlayerCharacterController::OnUnPossess()
{
	Super::OnUnPossess();
	EnhancedInputComponent->ClearActionBindings();
}

void APlayerCharacterController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (PlayerCharacter->GetCharacterMovement()->IsMovingOnGround() && !PlayerCharacter->GetVelocity().IsNearlyZero())
	{
		OnMove();
	}
	else
	{
		OnStopMove();
	}
}

void APlayerCharacterController::HandleLook(const FInputActionValue& InputActionValue)
{
	FVector2D lookInput = InputActionValue.Get<FVector2D>();
	
	if (bMovementBeingSlowed)
	{
		lookInput *= (1 - MovementSlowPercent);
	}

	AddYawInput(lookInput.X);
	AddPitchInput(lookInput.Y);
}

void APlayerCharacterController::HandleMove(const FInputActionValue& InputActionValue)
{
	if (!bAcceptMovementInput) return;
	FVector2D moveInput = InputActionValue.Get<FVector2D>();
	
	if (bMovementBeingSlowed)
	{
		moveInput *= (1 - MovementSlowPercent);
	}
	
	PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(),moveInput.X);
	PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(),moveInput.Y);
}

void APlayerCharacterController::HandleJump()
{
	PlayerCharacter->GetWeaponManager()->HandleJump();
	
	if (!bAcceptMovementInput) return;
	if (PlayerCharacter && PlayerCharacter->GetCharacterMovement()->IsFalling()) return;
	
	OnJump();
	PlayerCharacter->Jump();
}

void APlayerCharacterController::HandleInteract()
{
	if (!GetPawn()) return;

	// Get player viewpoint
	FVector start;
	FRotator viewRotation;
	GetPawn()->GetActorEyesViewPoint(start, viewRotation);
	
	FVector end = start + (viewRotation.Vector() * InteractRange); 

	FHitResult hitResult;

	// Perform the line trace 
	FCollisionQueryParams params;
	params.AddIgnoredActor(GetPawn()); 

	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, params);

	// Draw debug line
	DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 1.f, 0, 2.f);

	if (bHit && hitResult.GetActor())
	{
		// Check if the hit actor has an InteractableComponent
		UInteractableComponent* interactable = hitResult.GetActor()->FindComponentByClass<UInteractableComponent>();
		if (interactable)
		{
			// Trigger the interact event
			interactable->TriggerInteract(PlayerCharacter);
		}
	}
}
