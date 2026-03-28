#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"

/// Declare a dynamic multicast delegate with one parameter: the actor interacting
/// This allows Blueprints to bind events and receive the instigator
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, AActor*, InstigatorActor);

/// A reusable component that makes an actor "interactable".
/// and any Blueprint logic bound to OnInteract will execute.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MI498_UEPROJECT_API UInteractableComponent : public UActorComponent
{
public:
	/// Blueprint-assignable event.
	/// Blueprints can bind custom logic to this event.
	/// Fires when TriggerInteract() is called.
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnInteract OnInteract;
	
	/// Call this function to trigger the interaction event.
	/// All bound Blueprint events will be executed.
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void TriggerInteract(AActor* InstigatorActor)
	{
		OnInteract.Broadcast(InstigatorActor);
	}
	
	/// If this interactable should be allowed to be interacted with
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanInteractWith = true;
	
protected:
private:
	GENERATED_BODY()
};
