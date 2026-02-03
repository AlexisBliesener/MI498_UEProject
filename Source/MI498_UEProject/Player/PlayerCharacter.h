#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MI498_UEProject/Characters/CharacterBase.h"
#include "PlayerCharacter.generated.h"

class UWeaponManager;
class UCameraComponent;

/// Character class representing the player-controlled pawn.
///
/// Handles player-specific movement behavior such as walking and sprinting.
UCLASS()
class MI498_UEPROJECT_API APlayerCharacter : public ACharacterBase
{
public:
	
	APlayerCharacter();

	/// Toggles sprinting on or off.
	/// Updates the character movement speed based on the current sprint state.
	void ToggleSprint();
	
	// Called when ApplyDamage is used
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,AActor* DamageCauser) override;

	/// First person camera
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
	
	/// A Blueprintable function that will be called when the player lands on the ground
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerLanded();
	
	/// A Blueprintable function that will be called when the player starts sprinting
	UFUNCTION(BlueprintImplementableEvent)
	void OnSprint();
	
	/// Grants temporary invincibility for the specified duration
	/// @param Seconds - How long invincibility should last
	void AddInvincibility(float Seconds);
	
protected:
	
	virtual void Tick(float DeltaSeconds) override;
	
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
	/// Called automatically by the engine when the character lands on the ground
	/// Used to trigger Blueprint landing events
	virtual void Landed(const FHitResult& Hit) override;
	
	/// Tracks whether the player is currently sprinting
	bool bIsSprinting = false;
	
	/// Whether the player is currently invincible
	bool bIsInvincible = false;
	
	/// World time when invincibility expires
	float InvincibilityTimer = 0.0f;
	
	GENERATED_BODY()
};
