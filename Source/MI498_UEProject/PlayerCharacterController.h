#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

class UInputMappingContext;
class APlayerCharacter;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(PlayerLog, Log, All);

UCLASS(Abstract)
class MI498_UEPROJECT_API APlayerCharacterController : public APlayerController
{
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionMove = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionJump = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionSprint = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionLook = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;
	
protected:
	void HandleLook(const FInputActionValue& InputActionValue);
	void HandleMove(const FInputActionValue& InputActionValue);
	void HandleJump();
	void HandleSprint();
	
	virtual void OnPossess(APawn* PossessedPawn) override;
	virtual void OnUnPossess() override;
	
private:
	
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = nullptr;
	
	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter = nullptr;
	
	bool bIsSprinting = false;
	
	GENERATED_BODY()
};
