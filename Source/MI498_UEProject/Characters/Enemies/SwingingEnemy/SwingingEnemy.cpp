// Fill out your copyright notice in the Description page of Project Settings.


#include "SwingingEnemy.h"

#include "Components/StaticMeshComponent.h"
#include "CableComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "MI498_UEProject/AI/Components/EnemyMovementComponent.h"
#include "MI498_UEProject/Weapons/Throw/BombKnife.h"

// Sets default values
ASwingingEnemy::ASwingingEnemy(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UEnemyMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    AnchorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnchorMesh"));
    AnchorMesh->SetupAttachment(RootComponent);
    AnchorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AnchorMesh->SetHiddenInGame(true);

    SwingCable = CreateDefaultSubobject<UCableComponent>(TEXT("SwingCable"));
    SwingCable->SetupAttachment(AnchorMesh);
    SwingCable->PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
    SwingCable->CableGravityScale = 0.f;
    SwingCable->NumSegments = 4; 
    SwingCable->bEnableStiffness = true;
    SwingCable->SolverIterations = 1;
    SwingCable->EndLocation = FVector::ZeroVector;
}

void ASwingingEnemy::BeginPlay()
{
    Super::BeginPlay();
    
    CachedWorldPivot = GetActorTransform().TransformPosition(SwingCenterOffset);
    if (RealShip)
    {
        LocalSwingPivot = RealShip->GetActorTransform().InverseTransformPosition(CachedWorldPivot);
    }
    // This is used when the enemy wants to swing again, so they go to this point and then SWING
    GroundPointUnderSwing = GetGroundPointUnderSwing();
    // set the end of the cable to always follow the enemy mesh
    if (SwingCable && GetRootComponent())
    {
        SwingCable->SetAttachEndToComponent(GetRootComponent(), NAME_None);
    }

    AttachToSurface();
    
    if (ABombKnife* weapon = Cast<ABombKnife>(CurrentWeapon))
    {
        weapon->LaunchSpeed = LaunchSpeedBomb;
        weapon->KnifeAttackDamage = KnifeAttackDamage;
        weapon->KnifeAttackRadius = KnifeAttackRadius;
        weapon->ExplosionRadius = ExplosionRadius;
    }
    
    GridSizeEQS = AttackStartDistance - 300.f;
}

void ASwingingEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (RealShip)
    {
        CachedWorldPivot = RealShip->GetActorTransform().TransformPosition(LocalSwingPivot);
    }
    if (bIsShootingRope)
    {
        FVector currentAnchorLoc = AnchorMesh->GetComponentLocation();
        FVector toPivot = CachedWorldPivot - currentAnchorLoc;
        float distToPivot = toPivot.Size();
        
        float moveStep = RopeShootSpeed * DeltaTime;
        
        if (distToPivot <= moveStep)
        {
            AnchorMesh->SetWorldLocation(CachedWorldPivot);
            bIsShootingRope = false;
            RecordedCableLength = FVector::Distance(CachedWorldPivot, GetActorLocation());
            bIsReelingIn = true;
            bIsSwinging = true;
            
            if (UCharacterMovementComponent* moveComp = GetCharacterMovement())
            {
                moveComp->SetMovementMode(MOVE_Falling);
                moveComp->Velocity += FVector(0.f, 0.f, 250.f); 
            }
        }
        else
        {
            // move the anchor away to the ceiling
            AnchorMesh->SetWorldLocation(currentAnchorLoc + (toPivot.GetSafeNormal() * moveStep));
            // increate the cable length so it looks like it's going... 
            SwingCable->CableLength = FVector::Distance(GetActorLocation(), AnchorMesh->GetComponentLocation()) - 10.f;
        }
    }
    if (bIsSwinging)
    {
        HandleSwinging(DeltaTime);
    }
    

#if WITH_EDITOR
    if (GetWorld()->WorldType == EWorldType::Editor)
    {
        if (bDrawDebugSwinging )
        {
            DrawSwingPathEditor();
        }
        if (bDebug)
        {
            DrawCombatRangesEditor();
        }
    }
#endif
    
}
void ASwingingEnemy::AttachToSurface()
{
    if (!GetWorld() || !AnchorMesh || !SwingCable) return;

    FVector startLoc = GetActorLocation();
    if (RealShip)
    {
        AnchorMesh->AttachToComponent(RealShip->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
    }
    else
    {
        AnchorMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    }

    // Update point before setting it
    if (RealShip)
    {
        CachedWorldPivot = RealShip->GetActorTransform().TransformPosition(LocalSwingPivot);
    }
    AnchorMesh->SetWorldLocation(CachedWorldPivot);

    RecordedCableLength = CableLength;
    SwingCable->CableLength = RecordedCableLength -10.f;
    SwingCable->SetVisibility(true);
    
    FVector toEnemy = (startLoc - CachedWorldPivot).GetSafeNormal();
    SwingPlaneNormal = FVector::CrossProduct(FVector::DownVector, toEnemy).GetSafeNormal();
    
    if (SwingPlaneNormal.IsNearlyZero())
    {
        SwingPlaneNormal = GetActorRightVector();
    }
    // if we want the enemy to swing at right/left angle when the game starts   
    if (bStartAtMaxAngle)
    {
        float angleToRotate = bStartOnRightSide ? MaxSwingAngleDegrees : -MaxSwingAngleDegrees;
        
        FVector edgeDir = FVector::DownVector.RotateAngleAxis(angleToRotate, SwingPlaneNormal);
        
        FVector edgeLocation = CachedWorldPivot + (edgeDir * CableLength);
        
        SetActorLocation(edgeLocation);
        
        // remove momentum  so they start from a zero velocity 
        if (UCharacterMovementComponent* moveComp = GetCharacterMovement())
        {
            moveComp->Velocity = FVector::ZeroVector;
        }
    }
    bIsSwinging = true;
    
    if (UCharacterMovementComponent* moveComp = GetCharacterMovement())
    {
        moveComp->SetMovementMode(MOVE_Falling); 
    }
}

void ASwingingEnemy::Drop()
{
    bIsSwinging = false;
    
    if (SwingCable)
    {
        SwingCable->SetVisibility(false);
    }

    if (UCharacterMovementComponent* moveComp = GetCharacterMovement())
    {
        // set it back to walking so ai can move it 
        moveComp->SetMovementMode(MOVE_Walking);
    }


}

void ASwingingEnemy::HandleSwinging(float DeltaTime)
{
    if (!AnchorMesh) return;

    UCharacterMovementComponent* moveComp = GetCharacterMovement();
    if (!moveComp) return;
    if (bIsReelingIn)
    {
        RecordedCableLength -= RopeReelSpeed * DeltaTime;
        // we reached the targeted length 
        if (RecordedCableLength <= CableLength)
        {
            RecordedCableLength = CableLength;
            bIsReelingIn = false;
        }
        SwingCable->CableLength = RecordedCableLength - 10.f;
    }
    FVector anchorLoc = AnchorMesh->GetComponentLocation();
    FVector myLoc = GetActorLocation();
    FVector toAnchor = anchorLoc - myLoc;
    
    float currentDistance = toAnchor.Size();
    FVector toAnchorNormal = toAnchor.GetSafeNormal();
    FVector velocity = moveComp->Velocity;
    FVector planeVel = velocity - (FVector::DotProduct(velocity, SwingPlaneNormal) * SwingPlaneNormal);
    moveComp->Velocity = planeVel;

    float driftDist = FVector::DotProduct(myLoc - anchorLoc, SwingPlaneNormal);
    if (FMath::Abs(driftDist) > 5.f) 
    {
        SetActorLocation(myLoc - (SwingPlaneNormal * driftDist));
    }
    
    velocity = moveComp->Velocity;
    FVector radialVel = FVector::DotProduct(velocity, toAnchorNormal) * toAnchorNormal;
    
    if (currentDistance >= RecordedCableLength && FVector::DotProduct(velocity, toAnchorNormal) < 0.f)
    {
        moveComp->Velocity = velocity - radialVel;
    }

    if (currentDistance > RecordedCableLength + 5.f)
    {
        FVector correctedLoc = anchorLoc - (toAnchorNormal * RecordedCableLength);
        SetActorLocation(correctedLoc);
    }

    float currentAngleRad = FMath::Acos(FMath::Clamp(FVector::DotProduct(FVector::DownVector, -toAnchorNormal), -1.f, 1.f));
    float currentAngleDeg = FMath::RadiansToDegrees(currentAngleRad);

    float brakingZoneDegrees = 15.f;
    float startBrakeAngle = FMath::Max(0.f, MaxSwingAngleDegrees - brakingZoneDegrees);

    bool bMovingUpwards = moveComp->Velocity.Z > 0.f;

    if (currentAngleDeg > startBrakeAngle && bMovingUpwards)
    {
        float brakeAlpha = FMath::Clamp((currentAngleDeg - startBrakeAngle) / brakingZoneDegrees, 0.f, 1.f);
        
        moveComp->Velocity *= (1.0f - (brakeAlpha * DeltaTime * 10.f));

        if (currentAngleDeg >= MaxSwingAngleDegrees)
        {
            moveComp->Velocity = FVector::ZeroVector;
        }
    }
    else
    {
        // if the enemy doesn't move (Stuck for some reason) we will try to push him and hope he gets out 
        if (moveComp->Velocity.IsNearlyZero(15.f))
        {
            StuckTimer += DeltaTime;
            
            // if stuck longer than threshold then push!
            if (StuckTimer >= StuckTimeThreshold)
            {
                moveComp->SetMovementMode(MOVE_Falling);
                
                FVector kickDir = FVector::CrossProduct(SwingPlaneNormal, toAnchorNormal).GetSafeNormal();
                
                moveComp->Velocity += (kickDir * StuckPush);
                StuckTimer = 0.f; 
            }
        }
        else
        {
            // he's SWINGING 
            StuckTimer = 0.f;
            
            FVector moveDir = moveComp->Velocity.GetSafeNormal();
            moveComp->Velocity += (moveDir * SwingThrustForce * DeltaTime);
            
            if (currentAngleDeg < 5.f) 
            {
                FVector kickDir = FVector::CrossProduct(SwingPlaneNormal, toAnchorNormal).GetSafeNormal();
                moveComp->Velocity += (kickDir * SwingThrustForce * DeltaTime);
            }
        }
    }
}

void ASwingingEnemy::DetachAndJumpToGround(FVector TargetLocation)
{
    // drop the rope 
    Drop(); 

    FVector startLoc = GetActorLocation();
    UCharacterMovementComponent* moveComp = GetCharacterMovement();
    
    if (!moveComp || !GetWorld()) return;

    float gravity = FMath::Abs(GetWorld()->GetGravityZ()) * moveComp->GravityScale;
    if (gravity == 0.f)
    {
        gravity = 980.f; 
    }
    
    // get the highest point between the start and the target
    float highestPoint = FMath::Max(startLoc.Z, TargetLocation.Z);
    float topPointZ = highestPoint + 300.f; 
    
    float heightUp = topPointZ - startLoc.Z;
    float heightDown = topPointZ - TargetLocation.Z;
    
    float vectorZ = FMath::Sqrt(2.f * gravity * heightUp);
    
    float timeUp = vectorZ / gravity;
    float timeDown = FMath::Sqrt((2.f * heightDown) / gravity);
    
    float totalTimeInAir = timeUp + timeDown;

    FVector distanceXY = TargetLocation - startLoc;
    distanceXY.Z = 0.f; 
    
    if (totalTimeInAir > 0.f)
    {
        FVector vectorXY = distanceXY / totalTimeInAir;
        
        FVector launchVelocity = FVector(vectorXY.X, vectorXY.Y, vectorZ);

        moveComp->FallingLateralFriction = 0.f;
        moveComp->AirControl = 0.f;
        moveComp->BrakingDecelerationFalling = 0.f; 
        // jump 
        LaunchCharacter(launchVelocity, true, true);
    }
}
void ASwingingEnemy::ShootRopeAndSwing()
{
    if (!GetWorld() || !AnchorMesh) return;
    bIsShootingRope = true;
    bIsReelingIn = false;
    bIsSwinging = false;

    if (RealShip)
    {
        AnchorMesh->AttachToComponent(RealShip->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
    }
    else
    {
        AnchorMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    }
    AnchorMesh->SetWorldLocation(GetActorLocation());

    // make the rope 0 length so it grows 
    RecordedCableLength = 0.f;
    SwingCable->CableLength = 0.f;
    SwingCable->SetVisibility(true);
}

FVector ASwingingEnemy::GetGroundPointUnderSwing() const
{
    UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (navSys != nullptr)
    {
        FNavLocation groundLocation;
        
        FVector startPoint = CachedWorldPivot; 
        FVector localPivot = RealShip->GetActorTransform().InverseTransformPosition(startPoint);
        FVector fakePivot = HiddenShip->GetActorTransform().TransformPosition(localPivot);
        FVector lookBox = FVector(500.f, 500.f, 10000.f); 

        bool bFoundFloor = navSys->ProjectPointToNavigation(fakePivot, groundLocation, lookBox);
        
        if (bFoundFloor)
        {
            // translate the result from the nav mesh to the real ship 
            FVector localGround = HiddenShip->GetActorTransform().InverseTransformPosition(groundLocation.Location);
            FVector realGround = RealShip->GetActorTransform().TransformPosition(localGround);
            return realGround;
        }
    }

    // If it fails it will return the center of the world
    return FVector::ZeroVector; 
}

#if WITH_EDITOR
bool ASwingingEnemy::ShouldTickIfViewportsOnly() const
{
    return true; 
}
void ASwingingEnemy::DrawCombatRangesEditor()
{
    if (!bDebug) return;
    FVector center = GetActorLocation();
    UWorld* world = GetWorld();

    // Melee Range Yellow
    DrawDebugSphere(world, center, MeleeRange, 32, FColor::Yellow, false, -1.f, 0, 2.f);

    // Chase Range Blue
    DrawDebugSphere(world, center, ChaseRange, 32, FColor::Blue, false, -1.f, 0, 2.f);

    // Trigger Attack state RED
    DrawDebugSphere(world, center, AttackStartDistance, 32, FColor::Red, false, -1.f, 0, 2.f);
}
void ASwingingEnemy::DrawSwingPathEditor()
{
    FVector pivotLocation = GetActorTransform().TransformPosition(SwingCenterOffset);
    FVector startLoc = GetActorLocation();
    
    float ropeLength = CableLength;

    FVector toEnemy = (startLoc - pivotLocation).GetSafeNormal();
    FVector planeNormal = FVector::CrossProduct(FVector::DownVector, toEnemy).GetSafeNormal();
    
    if (planeNormal.IsNearlyZero())
    {
        planeNormal = GetActorRightVector();
    }
    // pivot location 
    DrawDebugSphere(GetWorld(), pivotLocation, 15.f, 12, FColor::Yellow, false, -1.f, 0, 2.f);

    int32 arcSegments = 20;
    float angleStep = (MaxSwingAngleDegrees * 2.f) / arcSegments;
    FVector prevArcPoint = FVector::ZeroVector;

    for (int32 i = 0; i <= arcSegments; ++i)
    {
        float currentAngle = -MaxSwingAngleDegrees + (angleStep * i);
        
        FVector arcDirection = FVector::DownVector.RotateAngleAxis(currentAngle, planeNormal);
        FVector arcPoint = pivotLocation + (arcDirection * ropeLength);

        if ( i > 0)
        {
            DrawDebugLine(GetWorld(), prevArcPoint, arcPoint, FColor::Cyan, false, -1.f, 0, 3.f);
        }
        prevArcPoint = arcPoint;

        if (i == 0 || i == arcSegments )
        {
            DrawDebugLine(GetWorld(), pivotLocation, arcPoint, FColor::Red, false, -1.f, 0, 1.f);
        }
    }

    DrawDebugLine(GetWorld(), pivotLocation, startLoc, FColor::White, false, -1.f, 0, 1.f);
}
#endif