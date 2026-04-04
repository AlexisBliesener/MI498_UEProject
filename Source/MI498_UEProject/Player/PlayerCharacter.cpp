#include "PlayerCharacter.h"

#include "PlayerCharacterController.h"
#include "../Weapons/WeaponManager.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/MissionSystem/SideMissionController.h"
#include "MI498_UEProject/ScoringSystem/ScoringManager.h"
#include "Perception/AIPerceptionComponent.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	/// Create weapon manager
	WeaponManager = CreateDefaultSubobject<UWeaponManager>(TEXT("Weapons Manger"));
	
	// Create raycast origins
	GrabRaycastOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("GrabRaycastOrigin"));
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
	DefaultSightCollisionChannel = GET_AI_CONFIG_VAR(DefaultSightCollisionChannel);
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,class AController* EventInstigator, AActor* DamageCauser)
{
	if (bDied) return DamageAmount;
	
	if (InvincibilityTimer >= GetWorld()->GetTimeSeconds())
	{
		return 0;
	}
	
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (CurrentHealth <= 0.f)
	{
		bDied = true;
		
		GetWorld()->GetTimerManager().ClearTimer(LowHealthTimer);
		TurnOffLowHealthEffect();
		
		APlayerCharacterController* playerController = Cast<APlayerCharacterController>(GetController());
		playerController->SetAcceptMovementInput(false);
		
		OnPlayerDied();
		return DamageAmount;
	}
	
	OnPlayerTakeDamage();
	
	/// Start low health effect if below threshold
	if (!bLowHealthHit && CurrentHealth <= MaxHealth * LowHealthPercentage)
	{
		bLowHealthHit = true;
		TurnOnLowHealthEffect();
		
		// Turn off low health effect after 4 seconds
		GetWorld()->GetTimerManager().SetTimer(
		LowHealthTimer,
		this,
		&APlayerCharacter::TurnOffLowHealthEffect,
		4.0f,
		false);
	}

	AActor* SourceActor = nullptr;

	// Prefer pawn instigator
	if (DamageCauser)
	{
		if (APawn* InstigatorPawn = DamageCauser->GetInstigator())
		{
			SourceActor = InstigatorPawn;
		}
	}

	// Fallback to pawn
	if (!SourceActor && EventInstigator && EventInstigator->GetPawn())
	{
		SourceActor = EventInstigator->GetPawn();
	}

	// fallback to actor
	if (!SourceActor && DamageCauser)
	{
		SourceActor = DamageCauser;
	}

	BP_OnDamageIndicator(SourceActor);
	
	return DamageAmount;
}

void APlayerCharacter::AddInvincibility(const float Seconds)
{
	InvincibilityTimer = GetWorld()->GetTimeSeconds() + Seconds;
}

UAISense_Sight::EVisibilityResult APlayerCharacter::CanBeSeenFrom(const FCanBeSeenFromContext& Context,FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested,float& OutSightStrength, int32* UserData, const FOnPendingVisibilityQueryProcessedDelegate* Delegate)
{
	FHitResult hitResult;
	// start checking against location first from the center of the character 
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(hitResult, Context.ObserverLocation, GetActorLocation(), DefaultSightCollisionChannel, FCollisionQueryParams(SCENE_QUERY_STAT(AILineOfSight), true, Context.IgnoreActor));
	OutNumberOfLoSChecksPerformed = 1;
	if(!bIsHit || hitResult.GetActor() != this)
	{
		// if we don't see the center of the character, we'll use the camera location (the player's point of view)
		bIsHit = GetWorld()->LineTraceSingleByChannel(hitResult, Context.ObserverLocation, Camera->GetComponentLocation(), DefaultSightCollisionChannel, FCollisionQueryParams(SCENE_QUERY_STAT(AILineOfSight), true, Context.IgnoreActor));
		OutNumberOfLoSChecksPerformed++;
	}
	OutSeenLocation = hitResult.Location;
	OutSightStrength = 1;
	return bIsHit && hitResult.GetActor() == this ? UAISense_Sight::EVisibilityResult::Visible : UAISense_Sight::EVisibilityResult::NotVisible;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	/// Set animation controller
	PlayerAnimation = Cast<UPlayerAnimation>(GetMesh()->GetAnimInstance());

	/// Set Scoring Manager
	ScoringManager = GetGameInstance()->GetSubsystem<UScoringManager>();
	
	// Turn off low health effect when the game starts.. 
	TurnOffLowHealthEffect();
}

void APlayerCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (bDied) return;
	
	/// Update speed on animation controller
	if (PlayerAnimation != nullptr)
	{
		PlayerAnimation->Speed = GetVelocity().Size();
		PlayerAnimation->SetInAir(GetCharacterMovement()->IsFalling());
		PlayerAnimation->SetJumped(false);
		PlayerAnimation->SetLookRotation(GetControlRotation().Pitch);
	}
	
	UpdateCameraOffset();

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
		ASideMissionController* SideMissionController = ASideMissionController::Get(this);
		SideMissionController->InAir();
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

void APlayerCharacter::HealCharacter(float HealAmount)
{
	Super::HealCharacter(HealAmount);
	
	if (bDied) return;
	
	if (CurrentHealth >= MaxHealth * LowHealthPercentage)
	{
		bLowHealthHit = false;
	}
}

void APlayerCharacter::Jump()
{
	Super::Jump();
	
	if (bDied) return;
	
	PlayerAnimation->SetJumped(true);
}

FGenericTeamId APlayerCharacter::GetGenericTeamId() const
{
	return FGenericTeamId(0);
}
void APlayerCharacter::GrabLedge(const FVector& TowardsLedge)
{
	if (bDied) return;
	
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
	OnPullUp();
	
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
	if (bDied) return;
	
	OnPlayerLanded();
	
	GetCharacterMovement()->GravityScale = 1.0f;
	
	ASideMissionController* SideMissionController = ASideMissionController::Get(this);
	SideMissionController->HitGround();
}

void APlayerCharacter::UpdateCameraOffset() const
{
	// Convert pitch to radians 
	const float pitchDeg = GetControlRotation().Pitch - 90.f;
	const float rad = FMath::DegreesToRadians(pitchDeg);

	// Calculate forward (X) and vertical (Z) 
	const float cosVal = -FMath::Cos(rad);
	const float sinVal = -(1 + FMath::Sin(rad));

	float newX;
	float newZ;

	// Adjust offsets depending on look direction
	if (cosVal > 0)
	{
		newX = cosVal * 25.f;
		newZ = sinVal * 50.f + 64.f;
	}
	else
	{
		newX = cosVal * 40.f;
		newZ = sinVal * 30.f + 64.f;
	}

	// Apply new relative camera position
	Camera->SetRelativeLocation(FVector(newX, 0.f, newZ));
}
