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
#include "../Player/PlayerCharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/AI/Components/SyncTransformOnHiddenShipComponent.h"
#if WITH_EDITOR
#include "EngineUtils.h"
#endif

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
            
            if (USyncTransformOnHiddenShipComponent* syncComponent = child->FindComponentByClass<USyncTransformOnHiddenShipComponent>())
            {
                syncComponent->HiddeActor = hiddenChild;
            }
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
    
    if (bIsActive)
    {
        if (APlayerCharacter* player = Cast<APlayerCharacter>( UGameplayStatics::GetPlayerPawn(GetWorld(), 0) ))
        {
            player->SetCurrentShip(this);
        }
    }
    
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

AEnemyBase* AShip::SpawnEnemyOnShip(TSubclassOf<AEnemyBase> Enemy, FTransform const& Transform, bool bIsActive)
{
    if (AEnemyBase* enemy = GetWorld()->SpawnActorDeferred<AEnemyBase>(Enemy,Transform,nullptr,nullptr,ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn))
    {
        enemy->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
        enemy->RealShip = this;
        enemy->HiddenShip = HiddenShip;
        UGameplayStatics::FinishSpawningActor(enemy, Transform);
        EnemiesOnShip.Add(enemy);
        if (bIsPlayerInside)
        { 
            enemy->SetEnabledEnemy(bIsActive);
        }
        if (!bIsActive)
        {
            PendingEnemies.Add(enemy);
        }
        SpawnedEnemies.Add(enemy);
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

void AShip::DestroyAllEnemiesOnShip()
{
    for (AEnemyBase* Enemy : EnemiesOnShip)
    {
        if (IsValid(Enemy))
        {
            Enemy->Destroy();
        }
    }

    EnemiesOnShip.Empty(); 
}


void AShip::TrySpawnEnemyUsingEQS(TArray<TSubclassOf<AEnemyBase>> EnemiesToSpawn, TArray<AActor*> EnemySpawnPoints, bool bIsActive)
{
    if (!SpawnEQS || !HiddenShip) return;

    FEnvQueryRequest spawnQueryRequest(SpawnEQS, this);


    FQueryFinishedSignature delegate = FQueryFinishedSignature::CreateUObject(this, &AShip::OnSpawnEQSFinished, EnemiesToSpawn, EnemySpawnPoints, bIsActive);

    spawnQueryRequest.Execute(EEnvQueryRunMode::AllMatching, delegate);
}

void AShip::OnSpawnEQSFinished(TSharedPtr<FEnvQueryResult> Result, TArray<TSubclassOf<AEnemyBase>> EnemiesToSpawn, TArray<AActor*> EnemySpawnPoints, bool bIsActive)
{
    if (!Result->IsSuccessful() || Result->Items.Num() == 0)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("EQS SPAWN FAILED !!!!!!"));
        }
        if (EnemySpawnPoints.Num() > 0)
        {
            for (int32 i = 0; i < EnemiesToSpawn.Num(); i++)
            {
                TSubclassOf<AEnemyBase> enemyClass = EnemiesToSpawn[i];
                if (!enemyClass) continue;

                AActor* fallbackPoint = EnemySpawnPoints[i % EnemySpawnPoints.Num()];
                if (IsValid(fallbackPoint))
                {
                    FTransform spawnTransform = fallbackPoint->GetActorTransform();
                    SpawnEnemyOnShip(enemyClass, spawnTransform, bIsActive);
                }
            }
        }
        return; 
    }

    int32 totalPoints = Result->Items.Num();
    
    for (int32 i = 0; i < EnemiesToSpawn.Num(); i++)
    {
        TSubclassOf<AEnemyBase> enemyClass = EnemiesToSpawn[i];
        if (!enemyClass) continue;

        FVector hiddenLocation = Result->GetItemAsLocation(i % totalPoints);
        
        FVector localPos = HiddenShip->GetActorTransform().InverseTransformPosition(hiddenLocation);
        FVector realLocation = GetActorTransform().TransformPosition(localPos);
        realLocation.Z += enemyClass.GetDefaultObject()->GetDefaultHalfHeight();
        FTransform spawnTransform;
        spawnTransform.SetLocation(realLocation);
        spawnTransform.SetRotation(FQuat::Identity); 

        SpawnEnemyOnShip(enemyClass, spawnTransform, bIsActive);
    }
}

void AShip::CheckAllActorsOnShip() const
{
#if WITH_EDITOR
    if (!IsValid(TraceCollisionBox) || (!GEngine)) return;
    
    int totalUnattached = 0;

    for (TActorIterator<AActor> it(GetWorld()); it; ++it)
    {
        AActor* foundActor = *it;

        if (!IsValid(foundActor) || foundActor == this) continue;
        // group actors are annoying.. so we don't want to include them :) 
        if (foundActor->GetClass()->GetName() == TEXT("GroupActor")) continue;

        if (TraceCollisionBox->Bounds.GetBox().IsInsideOrOn(foundActor->GetActorLocation()))
        {
            AActor* topParent = foundActor;
            while (topParent->GetAttachParentActor() != nullptr)
            {
                topParent = topParent->GetAttachParentActor();
            }

            if (topParent == this)
            {
                continue;
            }
            
            totalUnattached++;
            
            if (topParent != foundActor)
            {
                UE_LOG(LogTemp, Error, TEXT("%s: %s IS INSIDE THE CURRENT SHIP BUT IT'S ATTACHED TO %s !!!!!"), *GetActorNameOrLabel(),  *foundActor->GetActorNameOrLabel(),*topParent->GetActorNameOrLabel());
                continue;
            }
            UE_LOG(LogTemp, Error, TEXT("%s: %s IS INSIDE BUT NOT ATTACHED TO THE CURRENT SHIP!!!!!"), *GetActorNameOrLabel(),  *foundActor->GetActorNameOrLabel());
        }
    }

    UE_LOG(LogTemp, Error, TEXT("%s: %d ARE WRONG ATTACHED!!"), *GetActorNameOrLabel(), totalUnattached);
#endif
}
