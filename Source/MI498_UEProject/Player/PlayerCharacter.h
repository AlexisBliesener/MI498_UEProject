#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UWeaponManager;
/// Character class representing the player-controlled pawn.
///
/// Handles player-specific movement behavior such as walking and sprinting.
UCLASS()
class MI498_UEPROJECT_API APlayerCharacter : public ACharacter
{
public:
	
	APlayerCharacter();
	
	/// Toggles sprinting on or off.
	/// Updates the character movement speed based on the current sprint state.
	void ToggleSprint();

protected:
	
	/// Maximum walking speed when the player is not sprinting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CUSTOM Player|Movement" );
	int MaxWalkSpeed = 400;
	
	/// Maximum movement speed when the player is sprinting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CUSTOM Player|Movement");
	int MaxSprintSpeed = 800;
	
	/// Component responsible for managing the player's weapons
	/// Handles spawning, switching, and firing weapons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UWeaponManager> WeaponManager = nullptr;

private:
	/// Tracks whether the player is currently sprinting
	bool bIsSprinting = false;
	
	GENERATED_BODY()
};
