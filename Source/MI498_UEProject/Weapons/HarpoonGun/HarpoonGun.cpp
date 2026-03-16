#include "HarpoonGun.h"
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

	/// If using ADS exit it
	if (bUsingADS)
	{
		SecondaryAttackHoldEnd(Controller, Target);
	}

	/// Get starting location from the harpoon gun socket
	FTransform SocketTransform = PlayerCharacter->GetMesh()->GetSocketTransform("HarpoonGunBaseSocket");

	/// Get starting rotation from the camera direction
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	/// Configure spawn parameters for ownership and instigation
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = PlayerController->GetPawn();

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

void AHarpoonGun::PrimaryAttackHold(AController* Controller, AActor* Target)
{
	/// If holding past hold time threshold, enter reel in mode
	if (bHolding && GetWorld()->GetTimeSeconds() - HeldTime > HoldTime)
	{
		bSwingMode = false;
	}
}

void AHarpoonGun::PrimaryAttackHoldStart(AController* Controller, AActor* Target)
{
	Super::PrimaryAttackHoldStart(Controller, Target);

	/// Start timer to enter reel in mode
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
	PlayerCharacter->SetOverrideCameraFOV(true, ADSFOV);

	/// Slow Player Movement

	PlayerController->SetMovementSlow(true, 0.4f);
}

void AHarpoonGun::SecondaryAttackHoldEnd(AController* Controller, AActor* Target)
{
	Super::SecondaryAttackHoldEnd(Controller, Target);

	bUsingADS = false;

	/// Remove ADS
	PlayerCharacter->SetOverrideCameraFOV(false);

	/// Unslow Player Movement
	PlayerController->SetMovementSlow(false);
}

void AHarpoonGun::JumpAction()
{
	Super::JumpAction();

	/// Exit if the harpoon does not exist
	if (CurrentHarpoon == nullptr || CurrentHarpoon->GetReturningToPlayer() || !CurrentHarpoon->IsStuck())
	{
		return;
	}

	/// Add jump force
	if (PlayerCharacter)
	{
		FVector LaunchVelocity = FVector(0.f, 0.f, HarpoonReleaseJumpForce);
		PlayerCharacter->LaunchCharacter(LaunchVelocity, false, true);
	}

	/// Reload the harpoon
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

	/// Cache Player Character
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	PlayerCharacter->GetWeaponManager()->OnWeaponSwitch.AddDynamic(this, &AHarpoonGun::OnWeaponSwitched);

	/// Cache Player Controller
	PlayerController = Cast<APlayerCharacterController>(PlayerCharacter->GetController());
}

void AHarpoonGun::Tick(float DeltaSeconds)
{
	// Do not want base auto reload
	//Super::Tick(DeltaSeconds);
}

void AHarpoonGun::OnWeaponSwitched()
{
	AHarpoonGun* reference = Cast<AHarpoonGun>(PlayerCharacter->GetWeaponManager()->GetCurrentWeapon().GetObject());
	if (bUsingADS && reference == nullptr)
	{
		SecondaryAttackHoldEnd(GetInstigatorController(), nullptr);
	}
	
	if (CurrentHarpoon != nullptr)
	{
		CurrentHarpoon->ChangeSocketAttachment(reference != nullptr);
	}
}
