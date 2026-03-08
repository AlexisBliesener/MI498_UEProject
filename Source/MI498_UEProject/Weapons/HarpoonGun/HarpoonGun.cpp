#include "HarpoonGun.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/Player/PlayerCharacter.h"
#include "MI498_UEProject/Player/PlayerCharacterController.h"
#include "MI498_UEProject/Weapons/WeaponManager.h"

AHarpoonGun::AHarpoonGun()
{
	WeaponType = EWeaponType::HarpoonGun;
}

void AHarpoonGun::PrimaryAttack(AController* Controller, AActor* Target)
{
	if (CurrentHarpoon != nullptr) return;

	Super::PrimaryAttack(Controller, Target);

	if (bUsingADS)
	{
		SecondaryAttackHoldEnd(Controller, Target);
	}

	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		if (ACharacter* Character = Cast<ACharacter>(playerController->GetPawn()))
		{
			FTransform SocketTransform =
				Character->GetMesh()->GetSocketTransform("HarpoonGunBaseSocket");
		
			FVector CameraLocation;
			FRotator CameraRotation;
			playerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
		

			/// Configure spawn parameters for ownership and instigation
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = playerController->GetPawn();

			CurrentHarpoon = GetWorld()->SpawnActor<AHarpoon>(
		HarpoonBlueprint,
		SocketTransform.GetLocation(),
		CameraRotation,
		SpawnParams
	);

			/// Initialize harpoon properties after spawning
			CurrentHarpoon->SetRange(Range);
			CurrentHarpoon->SetHarpoonGun(this);
		}
	}
}

void AHarpoonGun::PrimaryAttackHold(AController* Controller, AActor* Target)
{
	/// No functionality

	if (bHolding && GetWorld()->GetTimeSeconds() - HeldTime > HoldTime)
	{
		bSwingMode = false;
	}
}

void AHarpoonGun::PrimaryAttackHoldStart(AController* Controller, AActor* Target)
{
	Super::PrimaryAttackHoldStart(Controller, Target);
	if (!bHolding)
	{
		bHolding = true;
		HeldTime = GetWorld()->GetTimeSeconds();
	}
}

void AHarpoonGun::PrimaryAttackHoldEnd(AController* Controller, AActor* Target)
{
	Super::PrimaryAttackHoldEnd(Controller, Target);
	bSwingMode = true;
	bHolding = false;
}

void AHarpoonGun::SecondaryAttack(AController* Controller, AActor* Target)
{
	Super::SecondaryAttack(Controller);
}

void AHarpoonGun::SecondaryAttackHoldStart(AController* Controller, AActor* Target)
{
	Super::SecondaryAttackHoldStart(Controller, Target);

	bUsingADS = true;

	/// ADS
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	playerCharacter->SetOverrideCameraFOV(true, ADSFOV);

	/// Slow Player Movement
	if (APlayerCharacterController* playerController = Cast<APlayerCharacterController>(Controller))
	{
		playerController->SetMovementSlow(true, 0.4f);
	}
}

void AHarpoonGun::SecondaryAttackHoldEnd(AController* Controller, AActor* Target)
{
	Super::SecondaryAttackHoldEnd(Controller, Target);

	bUsingADS = false;

	/// Remove ADS
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	playerCharacter->SetOverrideCameraFOV(false);

	/// Unslow Player Movement
	if (APlayerCharacterController* playerController = Cast<APlayerCharacterController>(Controller))
	{
		playerController->SetMovementSlow(false);
	}
}

void AHarpoonGun::JumpAction()
{
	Super::JumpAction();

	if (CurrentHarpoon == nullptr || CurrentHarpoon->GetReturningToPlayer() || !CurrentHarpoon->IsStuck())
	{
		return;
	}

	if (CharacterOwner)
	{
		FVector LaunchVelocity = FVector(0.f, 0.f, HarpoonReleaseJumpForce);
		CharacterOwner->LaunchCharacter(LaunchVelocity, false, true);
	}

	Reload();
}

void AHarpoonGun::DestroyCurrentHarpoon()
{
	CurrentHarpoon->Destroy();
	CurrentHarpoon = nullptr;
}

void AHarpoonGun::Reload()
{
	// Do not want base reload
	// Super::Reload();

	if (CurrentHarpoon == nullptr || CurrentHarpoon->GetReturningToPlayer())
	{
		return;
	}

	OnReload();

	CurrentHarpoon->ReturnToPlayer();
}

void AHarpoonGun::BeginPlay()
{
	Super::BeginPlay();

	CharacterOwner = Cast<APlayerCharacter>(GetOwner());

	CharacterOwner->GetWeaponManager()->OnWeaponSwitch.AddDynamic(this, &AHarpoonGun::OnWeaponSwitched);
}

void AHarpoonGun::Tick(float DeltaSeconds)
{
	// Do not want base auto reload
	//Super::Tick(DeltaSeconds);
}

void AHarpoonGun::OnWeaponSwitched()
{
	if (bUsingADS && !Cast<AHarpoonGun>(CharacterOwner->GetWeaponManager()->GetCurrentWeapon().GetObject()))
	{
		SecondaryAttackHoldEnd(GetInstigatorController(), nullptr);
	}
}
