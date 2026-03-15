#include "Ship.h"

#include "NavigationSystem.h"
#include "GameFramework/Actor.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Engine/TargetPoint.h"
#include "MI498_UEProject/AI/EnemyAIController.h"

AShip::AShip()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShip::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bFalling)
	{
		Fall(DeltaSeconds);
	}
}

void AShip::BeginPlay()
{
    Super::BeginPlay();

    DuplicateShipForNavigation();
}

void AShip::Fall(const float DeltaTime) 
{
	FVector FallOffset = FVector(0.f, 0.f, -FallSpeed * DeltaTime);
	AddActorWorldOffset(FallOffset, true);
    
    /// Tell all connected rowboats to fall
    OnShipFall.Broadcast(FallSpeed);
}

void AShip::DuplicateShipForNavigation()
{
    FVector goDown = FVector(-0.f, 0.f, -100000.f);
    FVector hiddenShipLocation = GetActorLocation() + goDown;
    FRotator hiddenShipRotation = GetActorRotation();
    
    FActorSpawnParameters shipSpawnParms;
    shipSpawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ATargetPoint* hiddenShipParent = GetWorld()->SpawnActor<ATargetPoint>(ATargetPoint::StaticClass(), hiddenShipLocation, hiddenShipRotation, shipSpawnParms);

    // Check if a NavMesh is attached to the real ship
    ANavMeshBoundsVolume* navMesh = nullptr;

    TArray<AActor*> attachedActorsOnShip;
    GetAttachedActors(attachedActorsOnShip);
    for (AActor* child : attachedActorsOnShip)
    {
        if (!child) continue;
        // Check if the child is a nav mesh 
        if (ANavMeshBoundsVolume* navMeshRef = Cast<ANavMeshBoundsVolume>(child))
        {
            navMesh = navMeshRef;
            continue; 
        }
        // Check if the child is enemy, it will just set the references to the ships and then go to the next child
        if (AEnemyBase* enemy = Cast<AEnemyBase>(child))
        {
            // Give the enemy the real ship and the fake ship
            enemy->RealShip = this; 
            enemy->HiddenShip = hiddenShipParent; 
            continue;
        }
        
        
        FActorSpawnParameters childSpawnParms;
        childSpawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        childSpawnParms.Template = child; 

        AActor* hiddenChild = GetWorld()->SpawnActor<AActor>(child->GetClass(), hiddenShipLocation, hiddenShipRotation, childSpawnParms);

        if (hiddenChild && hiddenShipParent)
        {
            hiddenChild->AttachToActor(hiddenShipParent, FAttachmentTransformRules::KeepRelativeTransform);
            
            // match transforms
            hiddenChild->SetActorRelativeLocation(child->GetRootComponent()->GetRelativeLocation());
            hiddenChild->SetActorRelativeRotation(child->GetRootComponent()->GetRelativeRotation());
            hiddenChild->SetActorRelativeScale3D(child->GetRootComponent()->GetRelativeScale3D());
            hiddenChild->SetActorHiddenInGame(true); 
            // hiddenChild->GetRootComponent()->SetCanEverAffectNavigation(true);
            hiddenChild->GetRootComponent()->UpdateBounds();
        }
    }

    // move the navmesh to the hidden ship 
    if (navMesh)
    {
        FVector originalRelativeLocation = navMesh->GetRootComponent()->GetRelativeLocation();
        FRotator originalRelativeRotation = navMesh->GetRootComponent()->GetRelativeRotation();
        FVector originalRelativeScale = navMesh->GetRootComponent()->GetRelativeScale3D();

        // Attach to the new hidden ship
        FAttachmentTransformRules navAttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false);
        navMesh->AttachToActor(hiddenShipParent, navAttachRules);
        
        // match transforms
        navMesh->SetActorRelativeLocation(originalRelativeLocation);
        navMesh->SetActorRelativeRotation(originalRelativeRotation);
        navMesh->SetActorRelativeScale3D(originalRelativeScale);

        // update the nav mesh 
        UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
        if (navSys)
        {
            navSys->OnNavigationBoundsUpdated(navMesh);
        }
    }
    else
    {
        UE_LOG(EnemyAILog, Error, TEXT("HEY no NavMesh found on Ship: %s !!!!!!!!!!"), *GetName());    
    }
}

