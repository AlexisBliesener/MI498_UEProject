// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_OtherBrutes.h"

#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "Kismet/GameplayStatics.h"
#include "MI498_UEProject/AI/Systems/EnemyManagerSubsystem.h"
#include "MI498_UEProject/Characters/Enemies/BruteEnemy.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"

void UEnvQueryContext_OtherBrutes::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
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

    TArray<FVector> locationsToReturn;

    if (enemy != nullptr)
    {
       bool bFoundSubsystem = false;

       if (UWorld* world = QueryInstance.World)
       {
          if (UEnemyManagerSubsystem* enemySubsystem = world->GetSubsystem<UEnemyManagerSubsystem>())
          {
             if (enemySubsystem->EnemiesAroundPlayer.Num() > 0)
             {
                bFoundSubsystem = true;
                
                for (AEnemyBase* otherEnemy : enemySubsystem->EnemiesAroundPlayer) 
                {
                   if (otherEnemy != nullptr && otherEnemy != enemy)
                   {
                      // if they have an assigned location then it is already on the fake ship navmesh
                      if (!otherEnemy->AssignedLocation.IsZero())
                      {
                         locationsToReturn.Add(otherEnemy->AssignedLocation);
                      }
                      // if no spot then get their real location and move it to fake ship
                      else if (otherEnemy->RealShip && otherEnemy->HiddenShip)
                      {
                         FVector localPos = otherEnemy->RealShip->GetActorTransform().InverseTransformPosition(otherEnemy->GetActorLocation());
                         FVector hiddenLocation = otherEnemy->HiddenShip->GetActorTransform().TransformPosition(localPos);

                         locationsToReturn.Add(hiddenLocation);
                      }
                   }
                }
             }
          }
       }

       // THIS IS FOR DEBUG EQS, it's not going to be used in the game 
       // if subsystem empty then find actors in editor
       if (!bFoundSubsystem)
       {
          TArray<AActor*> allEnemies;
          UGameplayStatics::GetAllActorsOfClass(QueryInstance.World, AEnemyBase::StaticClass(), allEnemies);

          for (AActor* actorInWorld : allEnemies)
          {
             if (actorInWorld != enemy && actorInWorld != querierActor)
             {
                if (AEnemyBase* enemyInWorld = Cast<AEnemyBase>(actorInWorld))
                {
                   if (!enemyInWorld->AssignedLocation.IsZero())
                   {
                      locationsToReturn.Add(enemyInWorld->AssignedLocation);
                   }
                   else if (enemyInWorld->RealShip && enemyInWorld->HiddenShip)
                   {
                      FVector localPos = enemyInWorld->RealShip->GetActorTransform().InverseTransformPosition(enemyInWorld->GetActorLocation());
                      FVector hiddenLocation = enemyInWorld->HiddenShip->GetActorTransform().TransformPosition(localPos);

                      locationsToReturn.Add(hiddenLocation);
                   }
                   else
                   {
                      
                      locationsToReturn.Add(enemyInWorld->GetActorLocation());
                   }
                }
             }
          }
       }
    }
    else
    {
       
       locationsToReturn.Add(querierActor->GetActorLocation());
    }

    UEnvQueryItemType_Point::SetContextHelper(ContextData, locationsToReturn);
}
