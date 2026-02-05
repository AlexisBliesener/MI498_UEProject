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
	
	/// A Blueprintable function that will be called when the player's health hits 0
	UFUNCTION(BlueprintImplementableEvent, Category="Player")
	void OnPlayerDied();
	
	/// Grants temporary invincibility for the specified duration
	/// @param Seconds - How long invincibility should last
	void AddInvincibility(float Seconds);
	
protected:
	
	virtual void Tick(float DeltaSeconds) override;
	
	/// Maximum walking speed when the player is not sprinting
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	int MaxWalkSpeed = 400;
	
	/// Component responsible for managing the player's weapons
	/// Handles spawning, switching, and firing weapons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UWeaponManager> WeaponManager = nullptr;

private:
	/// Called automatically by the engine when the character lands on the ground
	/// Used to trigger Blueprint landing events
	virtual void Landed(const FHitResult& Hit) override;
	
	/// Whether the player is currently invincible
	bool bIsInvincible = false;
	
	/// World time when invincibility expires
	float InvincibilityTimer = 0.0f;
	
	GENERATED_BODY()
};
