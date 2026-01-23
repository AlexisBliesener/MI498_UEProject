#include "WeaponManager.h"

#include "Blunderbuss.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.h"


DEFINE_LOG_CATEGORY(WeaponLog);


void UWeaponManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (WeaponBlueprints.Num() == 0 || !GetOwner())
	{
		UE_LOG(WeaponLog, Warning, TEXT("No weapon blueprints or no owner!"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	
	WeaponOptions.Empty();

	for (auto& WeaponBP : WeaponBlueprints)
	{
		if (!WeaponBP) continue;

		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
			WeaponBP,
			GetOwner()->GetActorLocation() + FVector(0,0,0),
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (SpawnedActor && SpawnedActor->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
		{
			WeaponOptions.Add(TScriptInterface<IWeaponInterface>(SpawnedActor));
		}
	}

	// Set the first weapon as current
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
	APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		UE_LOG(WeaponLog, Error, TEXT("Unable to get LocalPlayer"));
		return;
	}
	
	/// Retrieve the enhanced input subsystem for this local player
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!IsValid(InputSubsystem))
	{
		UE_LOG(WeaponLog, Error, TEXT("Unable to get reference to the UEnhancedInputLocalPlayerSubsystem"));
	}
	
	/// Reset existing input mappings and apply this controller's mapping context
	InputSubsystem->ClearAllMappings();
	InputSubsystem->AddMappingContext(InputMappingContext, 0);
	
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
	currentWeapon->Fire();
}

void UWeaponManager::HandleSecondaryAttack()
{
	UE_LOG(WeaponLog, Log, TEXT("secondary attack"));
}
