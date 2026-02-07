#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

class UInputMappingContext;
class APlayerCharacter;
class UInputAction;
struct FInputActionValue;

/// Log category for player controller–related messages
DECLARE_LOG_CATEGORY_EXTERN(PlayerLog, Log, All);

/// Player controller responsible for handling player input and forwarding
/// movement and action commands to the controlled PlayerCharacter.
///
/// Uses the Enhanced Input system to bind input actions such as movement,
/// camera look, sprinting, and jumping.
UCLASS(Abstract)
class MI498_UEPROJECT_API APlayerCharacterController : public APlayerController
{
public:
	/// Input action for character movement (WASD / left stick)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionMove = nullptr;
	
	/// Input action for jumping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionJump = nullptr;
	
	/// Input action for camera look (mouse / right stick)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionLook = nullptr;
	
	/// Input action for interacting with interactables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input")
	TObjectPtr<UInputAction> ActionInteract = nullptr;
	
	/// Input mapping context applied when this controller possesses a pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;
	
	/// A Blueprintable function that will be called when the player jumps
	UFUNCTION(BlueprintImplementableEvent)
	void OnJump();
	
	/// A Blueprintable function that will be called when the player is moving on the ground
	UFUNCTION(BlueprintImplementableEvent)
	void OnMove();
	
	/// A Blueprintable function that will be called when the player stops moving on the ground
	UFUNCTION(BlueprintImplementableEvent)
	void OnStopMove();
	
	/// Sets if the player is allowed to move 
	void SetAcceptMovementInput(const bool Val) { bAcceptMovementInput = Val; }
	
	/// The range that the player can interact from
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int InteractRange = 400;
	
protected:
	virtual void Tick(float DeltaSeconds) override;
	
	/// Handles camera look input and applies rotation to the controlled pawn
	/// @param InputActionValue Current look input value from the Enhanced Input system
	void HandleLook(const FInputActionValue& InputActionValue);
	
	/// Handles movement input to the PlayerCharacter
	/// @param InputActionValue Current movement input value from the Enhanced Input system
	void HandleMove(const FInputActionValue& InputActionValue);
	
	/// Handles jump input and triggers a jump on the PlayerCharacter
	void HandleJump();
	
	/// Handles interact input from the PlayerCharacter
	void HandleInteract();
	
	/// Called when this controller takes possession of a pawn
	/// Used to apply input mappings, and bind input actions
	virtual void OnPossess(APawn* PossessedPawn) override;
	
	/// Called when this controller releases possession of a pawn
	/// Used to clean up input bindings
	virtual void OnUnPossess() override;
	
private:
	/// Enhanced input component used for binding input actions
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = nullptr;
	
	/// Reference to the possessed PlayerCharacter
	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter = nullptr;
	
	/// True if the player is allowed to move
	bool bAcceptMovementInput = true;
	
	GENERATED_BODY()
};
