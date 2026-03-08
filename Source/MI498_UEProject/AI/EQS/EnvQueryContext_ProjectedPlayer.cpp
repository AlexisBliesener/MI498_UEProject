// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_ProjectedPlayer.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "GameFramework/PlayerStart.h"

void UEnvQueryContext_ProjectedPlayer::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
    AActor* querierActor = Cast<AActor>(QueryInstance.Owner.Get());
    if (!querierActor) return;

   AEnemyBase* enemy = Cast<AEnemyBase>(querierActor);
   // if the enemy is still empty that means the querier is the ai controller 
   if (enemy == nullptr) 
   {
      if (AController* controller = Cast<AController>(querierActor))
      {
         enemy = Cast<AEnemyBase>(controller->GetPawn());
      }
   }

    if (enemy != nullptr)
    {
       if (enemy->RealShip && enemy->HiddenShip)
       {
          APlayerController* playerController = UGameplayStatics::GetPlayerController(QueryInstance.World, 0);
          if (playerController && playerController->GetPawn())
          {
             FVector localPos = enemy->RealShip->GetActorTransform().InverseTransformPosition(playerController->GetPawn()->GetActorLocation());
             FVector hiddenLocation = enemy->HiddenShip->GetActorTransform().TransformPosition(localPos);
             UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(QueryInstance.World);
             if (navSys)
             {
                FNavLocation groundLocation;
                // find the floor this is help when the player is flyign 
                FVector lookBox = FVector(500.f, 500.f, 10000.f); 
                if (navSys->ProjectPointToNavigation(hiddenLocation, groundLocation, lookBox))
                {
                   hiddenLocation = groundLocation.Location;
                }
             }
             UEnvQueryItemType_Point::SetContextHelper(ContextData, hiddenLocation);
          }
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
