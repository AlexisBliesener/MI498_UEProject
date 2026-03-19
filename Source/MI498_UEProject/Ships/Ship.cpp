#include "Ship.h"

#include "NavigationSystem.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Engine/TargetPoint.h"
#include "Components/LightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/AI/EnemyAIController.h"

AShip::AShip()
{
	PrimaryActorTick.bCanEverTick = true;
    
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));;
    ActivationBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ActivationBox"));
    ActivationBox->SetupAttachment(RootComponent);
    ActivationBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ActivationBox->SetGenerateOverlapEvents(false);
    TraceCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TraceCollisionBox"));
    TraceCollisionBox->SetupAttachment(RootComponent);
    
    TraceCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    
    TraceCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TraceCollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
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
    // Create a HISM for each actor class that on the list 
    for (FHISMGroup& group : ActorsHISMOnShip)
    {
        if (group.ActorClass)
        {
            group.HISMComp = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
            group.HISMComp->SetMobility(EComponentMobility::Movable);
            group.HISMComp->SetGenerateOverlapEvents(false);
            
            group.HISMComp->RegisterComponent();
            group.HISMComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        }
    }

    GetAttachedActors(ActorsOnShip, true, true);
    // Start convert the actors to HISM 
    for (AActor* child : ActorsOnShip)
    {
        if (!child) continue;

        for (FHISMGroup& group : ActorsHISMOnShip)
        {
            if (group.ActorClass && child->IsA(group.ActorClass))
            {
                UStaticMeshComponent* meshComp = child->FindComponentByClass<UStaticMeshComponent>();
                
                if (meshComp && group.HISMComp)
                {
                    if (!group.bIsCopied && meshComp->GetStaticMesh())
                    {
                        group.HISMComp->SetStaticMesh(meshComp->GetStaticMesh());
                        // Copy the materials only once since they are all the same mesh.... 
                        int32 numMaterials = meshComp->GetNumMaterials();
                        for (int32 i = 0; i < numMaterials; i++)
                        {
                            if (UMaterialInterface* material = meshComp->GetMaterial(i))
                            {
                                group.HISMComp->SetMaterial(i, material);
                            }
                        }
                        group.bIsCopied = true; 
                    }

                    FTransform exactWorldTransform = meshComp->GetComponentTransform();
                    group.HISMComp->AddInstance(exactWorldTransform, true);
                }

                child->Destroy();
                break; 
            }
        }
    }
    

    
    GetWorldTimerManager().SetTimer(PlayerCheckTimer, this, &AShip::CheckPlayerBox, 0.5f, true);
    DuplicateShipForNavigation();
    
}

void AShip::Fall(const float DeltaTime) 
{
	FVector FallOffset = FVector(0.f, 0.f, -FallSpeed * DeltaTime);
    RootComponent->AddWorldOffset(FallOffset, false, nullptr, ETeleportType::TeleportPhysics);
    
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

    for (AActor* child : ActorsOnShip)
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
            EnemiesOnShip.Add(enemy);
            //enemy->SetEnabledEnemy(false);
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
    // disable ship as default 
    SetShipActive(false);
}

void AShip::CheckPlayerBox()
{
    if (APawn* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
    { 
        bool bIsInsideNow = ActivationBox->Bounds.GetBox().IsInsideOrOn(player->GetActorLocation());

        if (bIsInsideNow && !bIsPlayerInside)
        {
            SetShipActive(true);
            bIsPlayerInside = true; 
        }
        else if (!bIsInsideNow && bIsPlayerInside)
        {
            SetShipActive(false);
            bIsPlayerInside = false;
        }
    } 
}

void AShip::SetShipActive(bool bIsActive)
{
    
    // Stop trace collision when the player is on the ship 
    TraceCollisionBox->SetCollisionEnabled(bIsActive ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly);
    
    for (FHISMGroup& group : ActorsHISMOnShip)
    {
        if (group.HISMComp)
        {
            if (bIsActive)
            {
                group.HISMComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            }
            else
            {
                group.HISMComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
        }
    }
    
    for (AActor* child : ActorsOnShip)
    {
        if (IsValid(child))
        {
            if (ActorsIgnored.Contains(child))
            {
                continue; 
            }
            child->SetActorEnableCollision(bIsActive);
            child->SetActorTickEnabled(bIsActive);
            TArray<ULightComponent*> lightComponents;
            child->GetComponents(lightComponents);

            for (ULightComponent* light : lightComponents)
            {
                if (IsValid(light))
                {
                    light->SetVisibility(bIsActive);
                }
            }
        }
        
    }
    for (AEnemyBase* enemy : EnemiesOnShip)
    {
        if (IsValid(enemy))
        {
            enemy->SetEnabledEnemy(bIsActive);
        }
    }
}

void AShip::StartFalling()
{
    bFalling = true;
    if (UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
    {
        if (!navSys->IsNavigationBuildingLocked(1))
        {
            navSys->AddNavigationBuildLock(1);
        }
    }
}

