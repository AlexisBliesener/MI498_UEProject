#include "PlayerCharacterController.h"

#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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
	if (ActionSprint)
	{
		EnhancedInputComponent->BindAction(ActionSprint, ETriggerEvent::Triggered, this, &APlayerCharacterController::HandleSprint);
	}
}

void APlayerCharacterController::OnUnPossess()
{
	Super::OnUnPossess();
	EnhancedInputComponent->ClearActionBindings();
}

void APlayerCharacterController::HandleLook(const FInputActionValue& InputActionValue)
{
	const FVector2D lookInput = InputActionValue.Get<FVector2D>();

	AddYawInput(lookInput.X);
	AddPitchInput(lookInput.Y);
}

void APlayerCharacterController::HandleMove(const FInputActionValue& InputActionValue)
{
	const FVector2D moveInput = InputActionValue.Get<FVector2D>();
	
	PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(),moveInput.X);
	PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(),moveInput.Y);
}

void APlayerCharacterController::HandleJump()
{
	PlayerCharacter->Jump();
}

void APlayerCharacterController::HandleSprint()
{
	PlayerCharacter->ToggleSprint();
}
