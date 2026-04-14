// Fill out your copyright notice in the Description page of Project Settings.


#include "SyncTransformOnHiddenShipComponent.h"

#include "NavigationSystem.h"


// Sets default values for this component's properties
USyncTransformOnHiddenShipComponent::USyncTransformOnHiddenShipComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void USyncTransformOnHiddenShipComponent::SyncHiddenTransform(USceneComponent* SceneComponentToUpdate)
{
	if (IsValid(HiddeActor) && IsValid(SceneComponentToUpdate))
	{
		TArray<USceneComponent*> hiddenComponents;
		HiddeActor->GetComponents(hiddenComponents);

		for (USceneComponent* hiddenComponent : hiddenComponents)
		{
			if (hiddenComponent->GetFName() == SceneComponentToUpdate->GetFName())
			{
				hiddenComponent->SetRelativeTransform(SceneComponentToUpdate->GetRelativeTransform());

				// rebuild the nav mesh
				UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
				if (navSys)
				{
					navSys->UpdateComponentInNavOctree(*hiddenComponent);
				}
            
				return; 
			}
		}
	}
}

