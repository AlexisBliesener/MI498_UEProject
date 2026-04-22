// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_ProjectedPlayer.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "GameFramework/PlayerStart.h"
#include "MI498_UEProject/Ships/Ship.h"

void UEnvQueryContext_ProjectedPlayer::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
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

    if (realShip && hiddenShip)
    {
       APlayerController* playerController = UGameplayStatics::GetPlayerController(QueryInstance.World, 0);
       if (playerController && playerController->GetPawn())
       {
          FVector localPos = realShip->GetActorTransform().InverseTransformPosition(playerController->GetPawn()->GetActorLocation());
          FVector hiddenLocation = hiddenShip->GetActorTransform().TransformPosition(localPos);
          if (UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(QueryInstance.World))
          {
             FNavLocation location;
             FVector extent(10.0f, 10.0f, 1000.0f);
             
             if (navSys->ProjectPointToNavigation(hiddenLocation, location, extent))
             {
                hiddenLocation = location.Location;
             }
          }
          UEnvQueryItemType_Point::SetContextHelper(ContextData, hiddenLocation);
       }
    }
    else
    {
       // THIS IS FOR DEBUG (unreal method..)
       // it will use the "Player Start" as the player location for debugging
       if (AActor* playerStart = UGameplayStatics::GetActorOfClass(QueryInstance.World, APlayerStart::StaticClass()))
       {
          UEnvQueryItemType_Point::SetContextHelper(ContextData, playerStart->GetActorLocation());
       }
    }
}
