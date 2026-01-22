#include "PlayerCharacterController.h"

#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

DEFINE_LOG_CATEGORY(PlayerLog);

void APlayerCharacterController::OnPossess(APawn* PossessedPawn)
{
	Super::OnPossess(PossessedPawn);
	
	PlayerCharacter = Cast<APlayerCharacter>(PossessedPawn);
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(PlayerLog, Error, TEXT("APlayerCharacter can not be derived from the possessed pawn"));
	}
	
	EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(PlayerLog, Error, TEXT("Unable to get reference to the EnhancedInputComponent"));
	}
	
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem))
	{
		UE_LOG(PlayerLog, Error, TEXT("Unable to get reference to the UEnhancedInputLocalPlayerSubsystem"));
	}
	
	InputSubsystem->ClearAllMappings();
	InputSubsystem->AddMappingContext(InputMappingContext, 0);
	
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
	const FVector2D LookInput = InputActionValue.Get<FVector2D>();

	AddYawInput(LookInput.X);
	AddPitchInput(LookInput.Y);
}

void APlayerCharacterController::HandleMove(const FInputActionValue& InputActionValue)
{
	const FVector2D MoveInput = InputActionValue.Get<FVector2D>();
	
	PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(),MoveInput.X);
	PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(),MoveInput.Y);
}

void APlayerCharacterController::HandleJump()
{
	PlayerCharacter->Jump();
}

void APlayerCharacterController::HandleSprint()
{
	PlayerCharacter->ToggleSprint();
}
