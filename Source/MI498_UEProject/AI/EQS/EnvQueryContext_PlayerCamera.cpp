// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_PlayerCamera.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "GameFramework/PlayerStart.h"
#include "MI498_UEProject/Ships/Ship.h"

void UEnvQueryContext_PlayerCamera::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
    AActor* querierActor = Cast<AActor>(QueryInstance.Owner.Get());
    if (!querierActor) return;

    AEnemyBase* enemy = Cast<AEnemyBase>(querierActor);
    AActor* realShip = nullptr;
    AActor* hiddenShip = nullptr;

    // if the enemy is still empty that means the querier is the ai controller 
    if (enemy == nullptr) 
    {
       if (AController* controller = Cast<AController>(querierActor))
       {
          enemy = Cast<AEnemyBase>(controller->GetPawn());
       }
    }
    
    if (enemy == nullptr)
    {
       if (AShip* ship = Cast<AShip>(querierActor))
       {
          realShip = ship;
          hiddenShip = ship->GetHiddenShip();
       }
    }
    else
    {
       realShip = enemy->RealShip;
       hiddenShip = enemy->HiddenShip;
    }

    TArray<FVector> contextLocations;

    if (realShip && hiddenShip)
    {
       APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(QueryInstance.World, 0);
       APawn* playerPawn = UGameplayStatics::GetPlayerPawn(QueryInstance.World, 0);

       if (cameraManager && playerPawn)
       {
          FVector localPlayerPos = realShip->GetActorTransform().InverseTransformPosition(playerPawn->GetActorLocation());
          FVector hiddenPlayerLocation = hiddenShip->GetActorTransform().TransformPosition(localPlayerPos);
          contextLocations.Add(hiddenPlayerLocation);

          FVector localCameraPos = realShip->GetActorTransform().InverseTransformPosition(cameraManager->GetCameraLocation());
          FVector hiddenCameraLocation = hiddenShip->GetActorTransform().TransformPosition(localCameraPos);
          contextLocations.Add(hiddenCameraLocation);
          UEnvQueryItemType_Point::SetContextHelper(ContextData, contextLocations);
       }
    }
   
    if (contextLocations.Num() == 0) 
    {
       // Debugginggg… boring.
       if (AActor* playerStart = UGameplayStatics::GetActorOfClass(QueryInstance.World, APlayerStart::StaticClass()))
       {
          UEnvQueryItemType_Point::SetContextHelper(ContextData, playerStart->GetActorLocation());
       }
    }
}