#include "Ship.h"

#include "NavigationSystem.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Engine/TargetPoint.h"
#include "Components/LightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/NavLinkProxy.h"
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
    TraceCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
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
    // We should duplicate the ship before we convert the actors on the ship to HISM so the navmesh build on the actors before the conversion! 
    DuplicateShipForNavigation();
    // Start convert the actors to HISM 
    ConvertSMToHISM();
    
    GetWorldTimerManager().SetTimer(PlayerCheckTimer, this, &AShip::CheckPlayerBox, 0.05f, true);
    
}

void AShip::Fall(const float DeltaTime)
{
    if (bIsPlayerInside)
    {
        FVector FallOffset = FVector(0.f, 0.f, -FallSpeed * DeltaTime);
        RootComponent->AddWorldOffset(FallOffset, false, nullptr, ETeleportType::TeleportPhysics);
        OnShipFall.Broadcast(FallSpeed);
    }
    else
    {
        // Stagger updates across frames using ship index
        if (GFrameCounter % 8 != ShipIndex)
            return;

        FallAccumulator += DeltaTime * 8.f;
        if (FallAccumulator < FallInterval)
            return;

        FVector FallOffset = FVector(0.f, 0.f, -FallSpeed * FallAccumulator);
        RootComponent->AddWorldOffset(FallOffset, false, nullptr, ETeleportType::TeleportPhysics);
        OnShipFall.Broadcast(FallSpeed);

        FallAccumulator = 0.f;
    }

}

void AShip::DuplicateShipForNavigation()
{
    FVector goDown = FVector(-0.f, 0.f, -100000.f);
    FVector hiddenShipLocation = GetActorLocation() + goDown;
    FRotator hiddenShipRotation = GetActorRotation();
    
    FActorSpawnParameters shipSpawnParms;
    shipSpawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    HiddenShip = GetWorld()->SpawnActor<ATargetPoint>(ATargetPoint::StaticClass(), hiddenShipLocation, hiddenShipRotation, shipSpawnParms);
    // Check if a NavMesh is attached to the real ship
    ANavMeshBoundsVolume* navMesh = nullptr;

    // we need to move the nav links in the ships to the fake ship so it works with the ai navigation 
    TArray<ANavLinkProxy*> navLinks;
    for (AActor* child : ActorsOnShip)
    {
        if (!child) continue;
        // Check if the child is a nav mesh 
        if (ANavMeshBoundsVolume* navMeshRef = Cast<ANavMeshBoundsVolume>(child))
        {
            navMesh = navMeshRef;
            continue; 
        }
        if (ANavLinkProxy* navLinkRef = Cast<ANavLinkProxy>(child))
        {
            navLinks.Add(navLinkRef);
            continue; 
        }
        // Check if the child is enemy, it will just set the references to the ships and then go to the next child
        if (AEnemyBase* enemy = Cast<AEnemyBase>(child))
        {
            // Give the enemy the real ship and the fake ship
            enemy->RealShip = this; 
            enemy->HiddenShip = HiddenShip; 
            EnemiesOnShip.Add(enemy);
            //enemy->SetEnabledEnemy(false);
            continue;
        }
        if (Cast<AController>(child))
        {
            // we don't need to copy the controller since it would be already attached to the enemy....
            continue;
        }
        
        FActorSpawnParameters childSpawnParms;
        childSpawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        childSpawnParms.Template = child; 

        AActor* hiddenChild = GetWorld()->SpawnActor<AActor>(child->GetClass(), hiddenShipLocation, hiddenShipRotation, childSpawnParms);

        if (hiddenChild && HiddenShip)
        {
            hiddenChild->AttachToActor(HiddenShip, FAttachmentTransformRules::KeepRelativeTransform);
            
            // match transforms
            hiddenChild->SetActorRelativeLocation(child->GetRootComponent()->GetRelativeLocation());
            hiddenChild->SetActorRelativeRotation(child->GetRootComponent()->GetRelativeRotation());
            hiddenChild->SetActorRelativeScale3D(child->GetRootComponent()->GetRelativeScale3D());
            hiddenChild->SetActorHiddenInGame(true); 
            // hiddenChild->GetRootComponent()->SetCanEverAffectNavigation(true);
            hiddenChild->GetRootComponent()->UpdateBounds();
        }
        TArray<UPrimitiveComponent*> primitiveComponents;
        child->GetComponents(primitiveComponents);
        for (UPrimitiveComponent* primitiveComponent : primitiveComponents)
        {
            if (primitiveComponent)
            {
                // we don't need this actor to affect the navigation anymore because we have a different instance of this actor in the fake ship.. 
                // even though i locked the build (i know nothing about this function i just found a function in the source code called AddNavigationBuildLock
                // and the name feels like it does something but i couldn't find any official document about it) 
                // but disabling "SetCanEverAffectNavigation" for some reason gives us a little bit of a good performance :) 
                primitiveComponent->SetCanEverAffectNavigation(false); 
            }
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
        navMesh->AttachToActor(HiddenShip, navAttachRules);
        
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
    
    for (ANavLinkProxy* navLink : navLinks)
    {
        FVector originalRelativeLocation = navLink->GetRootComponent()->GetRelativeLocation();
        FRotator originalRelativeRotation = navLink->GetRootComponent()->GetRelativeRotation();
        FVector originalRelativeScale = navLink->GetRootComponent()->GetRelativeScale3D();

        FAttachmentTransformRules navAttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false);
        // reattach it to the actor of the hidden ship and move it to the same location that is in the real ship
        navLink->AttachToActor(HiddenShip, navAttachRules);
        navLink->SetActorRelativeLocation(originalRelativeLocation);
        navLink->SetActorRelativeRotation(originalRelativeRotation);
        navLink->SetActorRelativeScale3D(originalRelativeScale);

        UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
        if (navSys)
        {
            // sicne we moved the nav link we need to update the navigation system to see the new chagnes....
            navSys->UpdateActorAndComponentsInNavOctree(*navLink);
        }
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
            bIsCannonAiming = false;
        }
        else if (!bIsInsideNow && bIsPlayerInside)
        {
            bIsPlayerInside = false;

            // ONLY deactivate the ship if the cannon is NOT currently aiming at the ship 
            if (!bIsCannonAiming)
            {
                SetShipActive(false);
            }
        }
    }
}

void AShip::ConvertSMToHISM()
{
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

AEnemyBase* AShip::SpawnEnemyOnShip(TSubclassOf<AEnemyBase> Enemy, FTransform const& Transform)
{
    if (AEnemyBase* enemy = GetWorld()->SpawnActorDeferred<AEnemyBase>(Enemy,Transform))
    {
        enemy->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
        enemy->RealShip = this;
        enemy->HiddenShip = HiddenShip;
        UGameplayStatics::FinishSpawningActor(enemy, Transform);
        EnemiesOnShip.Add(enemy);
        //enemy->SetEnabledEnemy(bIsPlayerInside);
        return enemy;
    }
    UE_LOG(EnemyLog, Error, TEXT("Enemy spawned isnt real. Ship: %s"), *GetName());
    return nullptr;
}

void AShip::SetCannonAiming(bool bIsAiming, AShip* LastShipActivated)
{
    if (IsValid(LastShipActivated))
    {
        // avoid duplicated calls if it's already active...
        if (LastShipActivated == this && bIsCannonAiming)
        {
            return;
        }
        // if the last ship is not this ship then disable the old ship 
        if (LastShipActivated != this)
        {
            LastShipActivated->SetCannonAiming(false, nullptr);
        }
    }
    
    bIsCannonAiming = bIsAiming;
    
    if (bIsPlayerInside)
    {
        return;
    }
    
    
    if (bIsCannonAiming && !bIsPlayerInside)
    {
        SetShipActive(true);
    }
    else if (!bIsCannonAiming && !bIsPlayerInside)
    {
        SetShipActive(false);
    }
}

