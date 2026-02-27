#include "PlayerCharacter.h"

#include "PlayerCharacterController.h"
#include "../Weapons/WeaponManager.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MI498_UEProject/ScoringSystem/ScoringManager.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	/// Create weapon manager
	WeaponManager = CreateDefaultSubobject<UWeaponManager>(TEXT("Weapons Manger"));
	
	// Create raycast origins
	GrabRaycastOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("GrabRaycastOrigin test name"));
	GrabRaycastOrigin->SetupAttachment(RootComponent);
	BodyRaycastOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("BodyRaycastOrigin"));
	BodyRaycastOrigin->SetupAttachment(RootComponent);
	
	/// Add first person camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	Camera->SetupAttachment(RootComponent);
	Camera->bUsePawnControlRotation = true;
	Camera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	// ECC_GameTraceChannel1 is only for the player
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,class AController* EventInstigator, AActor* DamageCauser)
{
	if (InvincibilityTimer >= GetWorld()->GetTimeSeconds())
	{
		return 0;
	}
	
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (CurrentHealth <= 0.f)
	{
		OnPlayerDied();
	}
	
	OnPlayerTakeDamage();
	
	return DamageAmount;
}

void APlayerCharacter::AddInvincibility(const float Seconds)
{
	InvincibilityTimer = GetWorld()->GetTimeSeconds() + Seconds;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	/// Set animation controller
	PlayerAnimation = Cast<UPlayerAnimation>(GetMesh()->GetAnimInstance());

	/// Set Scoring Manager
	ScoringManager = GetGameInstance()->GetSubsystem<UScoringManager>();
}

void APlayerCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	/// Update speed on animation controller
	PlayerAnimation->Speed = GetVelocity().Size();

	// Velocity cap
	const FVector velocity = GetVelocity();
	if (velocity.Size() > MaxVelocity)
	{
		GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity.GetClampedToMaxSize(MaxVelocity);
	}
	
	// FOV change based on velocity
	if (bOverrideCameraFOV)
	{
		Camera->SetFieldOfView(FMath::Lerp(Camera->FieldOfView, OverrideCameraFOV, LerpToNewFOVSpeed));
	}
	else if (FOVCurve && Camera)
	{
		float SpeedAlpha = FMath::Clamp(velocity.Size() / MaxVelocity, 0.f, 1.f);

		float CurveAlpha = FOVCurve->GetFloatValue(SpeedAlpha);

		float NewFOV = FMath::Lerp(MinFOV, MaxFOV, CurveAlpha);

		Camera->SetFieldOfView(FMath::Lerp(Camera->FieldOfView, NewFOV, LerpToNewFOVSpeed));
	}
	
	/// Set if player is in air for score
	if (GetCharacterMovement()->IsFalling())
	{
		ScoringManager->SetInAir(true);
	}
	else
	{
		ScoringManager->SetInAir(false);
	}
	
	// Decide if the player can grab ledge
	if (!GetCharacterMovement()->IsMovingOnGround() && !bIsGrabbing)
	{
		// Upper trace — checks if space above ledge is clear
		FVector grabStart = GrabRaycastOrigin->GetComponentLocation();
		FVector grabEnd = grabStart + GrabRaycastOrigin->GetForwardVector() * MinLedgeSize;
		FHitResult grabHit;
		bool grabResult = GetWorld()->LineTraceSingleByChannel(grabHit, grabStart, grabEnd, ECC_Visibility);
	
		// Lower trace — checks if wall exists in front of body
		FVector bodyStart = BodyRaycastOrigin->GetComponentLocation();
		FVector bodyEnd = bodyStart + BodyRaycastOrigin->GetForwardVector() * MaxDistanceFromLedge;
		FHitResult bodyHit;
		bool bodyResult = GetWorld()->LineTraceSingleByChannel(bodyHit, bodyStart, bodyEnd, ECC_Visibility);
	
		// If wall detected but upper space is clear, ledge detected
		if (bodyResult && !grabResult)
		{
			GrabLedge(BodyRaycastOrigin->GetForwardVector());
		}
	}
}

void APlayerCharacter::GrabLedge(const FVector& TowardsLedge)
{
	OnPlayerGrabLedge();
	
	// Get custom player controller and disable movement input
	APlayerCharacterController* playerController = Cast<APlayerCharacterController>(GetController());
	playerController->SetAcceptMovementInput(false);
	
	bIsGrabbing = true;

	// stop movement and gravity
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->StopMovementImmediately();
	
	FTimerDelegate delegate;
	delegate.BindUFunction(this, FName("PullUp"), TowardsLedge);
	
	GetWorld()->GetTimerManager().SetTimer(
	TimerHandle,
	delegate,
	0.1f,   
	false);
}

void APlayerCharacter::PullUp(const FVector& TowardsLedge)
{
	LaunchCharacter(GetActorUpVector() * PullUpToLedgeForce, true, true);
	
	FTimerDelegate delegate;
	delegate.BindUFunction(this, FName("StepForward"), TowardsLedge);
	
	GetWorld()->GetTimerManager().SetTimer(
	TimerHandle,
	delegate,
	0.5f,   
	false  
);
}

void APlayerCharacter::StepForward(const FVector& TowardsLedge)
{
	LaunchCharacter(TowardsLedge * StepForwardToLedgeForce, true, true);
	GetWorld()->GetTimerManager().SetTimer(
	TimerHandle,
	this,
	&APlayerCharacter::ReenableMovement,
	0.1f,   
	false  
);

}

void APlayerCharacter::ReenableMovement()
{
	APlayerCharacterController* playerController = Cast<APlayerCharacterController>(GetController());
	playerController->SetAcceptMovementInput(true);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	bIsGrabbing = false;
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	OnPlayerLanded();
}
