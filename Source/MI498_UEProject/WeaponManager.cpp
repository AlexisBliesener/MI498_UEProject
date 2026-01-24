#include "WeaponManager.h"

#include "Blunderbuss.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.h"

/// Define a custom logging category for weapon manager messages
DEFINE_LOG_CATEGORY(WeaponLog);


void UWeaponManager::BeginPlay()
{
	Super::BeginPlay();
	
	/// Ensure there is weapon blueprints to spawn and an owner for this manager
	if (WeaponBlueprints.Num() == 0 || !GetOwner())
	{
		UE_LOG(WeaponLog, Warning, TEXT("No weapon blueprints or no owner!"));
		return;
	}

	/// Set up spawn parameters so the weapons know their owner
	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = GetOwner();
	
	/// Clear any previous weapon options to prepare for spawning
	WeaponOptions.Empty();

	/// Spawn each weapon blueprint and store as an interface
	for (auto& weaponBP : WeaponBlueprints)
	{
		if (!weaponBP) continue;

		/// Spawn weapon actor at the owner's location
		AActor* spawnedActor = GetWorld()->SpawnActor<AActor>(
			weaponBP,
			GetOwner()->GetActorLocation() + FVector(0,0,0),
			FRotator::ZeroRotator,
			spawnParameters
		);
		
		/// Ensure weapons implement the IWeaponInterface and add them to WeaponOptions
		if (spawnedActor && spawnedActor->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
		{
			WeaponOptions.Add(TScriptInterface<IWeaponInterface>(spawnedActor));
		}
	}

	/// Set the first weapon as the current active weapon
	if (WeaponOptions.Num() > 0)
	{
		currentWeapon = WeaponOptions[0];
		UE_LOG(WeaponLog, Log, TEXT("Current weapon: %s"), *currentWeapon.GetObject()->GetName());
	}
	
	/// Cache the possessed player character
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(WeaponLog, Error, TEXT("APlayerCharacter can not be derived from the possessed pawn"));
	}
	
	/// Cache the enhanced input component for action binding
	EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerCharacter->InputComponent);
	if (!IsValid(EnhancedInputComponent))
	{
		UE_LOG(WeaponLog, Error, TEXT("Unable to get reference to the EnhancedInputComponent"));
	}
	
	/// Get local player
	APlayerController* playerController = Cast<APlayerController>(PlayerCharacter->GetController());
	ULocalPlayer* localPlayer = playerController->GetLocalPlayer();
	if (!IsValid(localPlayer))
	{
		UE_LOG(WeaponLog, Error, TEXT("Unable to get LocalPlayer"));
		return;
	}
	
	/// Retrieve the enhanced input subsystem for this local player
	UEnhancedInputLocalPlayerSubsystem* inputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(localPlayer);
	if (!IsValid(inputSubsystem))
	{
		UE_LOG(WeaponLog, Error, TEXT("Unable to get reference to the UEnhancedInputLocalPlayerSubsystem"));
	}
	
	/// Reset existing input mappings and apply this controller's mapping context
	inputSubsystem->ClearAllMappings();
	inputSubsystem->AddMappingContext(InputMappingContext, 0);
	
	/// Bind inputs
	if (ActionWeaponOne)
	{
		EnhancedInputComponent->BindAction(ActionWeaponOne, ETriggerEvent::Triggered, this, &UWeaponManager::HandleSelectWeaponOne);
	}
	if (ActionWeaponTwo)
	{
		EnhancedInputComponent->BindAction(ActionWeaponTwo, ETriggerEvent::Triggered, this, &UWeaponManager::HandleSelectWeaponTwo);
	}
	if (ActionWeaponThree)
	{
		EnhancedInputComponent->BindAction(ActionWeaponThree, ETriggerEvent::Triggered, this, &UWeaponManager::HandleSelectWeaponThree);
	}
	if (ActionWeaponPrev)
	{
		EnhancedInputComponent->BindAction(ActionWeaponPrev, ETriggerEvent::Triggered, this, &UWeaponManager::HandleSelectWeaponPrev);
	}
	if (ActionWeaponNext)
	{
		EnhancedInputComponent->BindAction(ActionWeaponNext, ETriggerEvent::Triggered, this, &UWeaponManager::HandleSelectWeaponNext);
	}
	if (ActionPrimaryAttack)
	{
		EnhancedInputComponent->BindAction(ActionPrimaryAttack, ETriggerEvent::Triggered, this, &UWeaponManager::HandlePrimaryAttack);
	}
	if (ActionSecondaryAttack)
	{
		EnhancedInputComponent->BindAction(ActionSecondaryAttack, ETriggerEvent::Triggered, this, &UWeaponManager::HandleSecondaryAttack);
		
	}
}

void UWeaponManager::HandleSelectWeaponOne()
{
	UE_LOG(WeaponLog, Log, TEXT("weapon one"));
	UE_LOG(WeaponLog, Log, TEXT("Current Weapon: %s"), *currentWeapon.GetObject()->GetName());
}

void UWeaponManager::HandleSelectWeaponTwo()
{
	UE_LOG(WeaponLog, Log, TEXT("weapon two"));
	UE_LOG(WeaponLog, Log, TEXT("Current Weapon: %s"), *currentWeapon.GetObject()->GetName());
}

void UWeaponManager::HandleSelectWeaponThree()
{
	UE_LOG(WeaponLog, Log, TEXT("weapon three"));
	UE_LOG(WeaponLog, Log, TEXT("Current Weapon: %s"), *currentWeapon.GetObject()->GetName());
}

void UWeaponManager::HandleSelectWeaponPrev()
{
	UE_LOG(WeaponLog, Log, TEXT("weapon prev"));
	UE_LOG(WeaponLog, Log, TEXT("Current Weapon: %s"), *currentWeapon.GetObject()->GetName());
}

void UWeaponManager::HandleSelectWeaponNext()
{
	UE_LOG(WeaponLog, Log, TEXT("weapon next"));
	UE_LOG(WeaponLog, Log, TEXT("Current Weapon: %s"), *currentWeapon.GetObject()->GetName());
}

void UWeaponManager::HandlePrimaryAttack()
{
	UE_LOG(WeaponLog, Log, TEXT("primary attack"));
	/// Call the weapon's PrimaryAttack function, passing the player controller
	currentWeapon->PrimaryAttack(Cast<APlayerController>(PlayerCharacter->GetController()));
}

void UWeaponManager::HandleSecondaryAttack()
{
	UE_LOG(WeaponLog, Log, TEXT("secondary attack"));
}
