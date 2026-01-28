// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeAIComponent.h"
#include "StateTreeEnemyComponent.generated.h"


/**
 * A custom state tree class to start the tree with custom state tree logics
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent, DisplayName = "State Tree AI Component"))
class MI498_UEPROJECT_API UStateTreeEnemyComponent : public UStateTreeAIComponent
{
	GENERATED_BODY()

public:
	/**
	 * Constructor for the UStateTreeEnemyComponent class.
	 * Initializes component properties for tick functionality.
	 */
	UStateTreeEnemyComponent();

	/**
	 * Starts the assigned state tree with a custom state tree
	 * 
	 * @param StateTree A pointer to the state tree that will be started. If the state tree is already running, it will stop and restart it with the provided state tree reference
	 */
	UFUNCTION(BlueprintCallable, Category = "StateTree", meta = (AutoCreateRefTerm = "StateTreeReference", DisplayName = "Start State Tree"))
	void StartStateTree(UStateTree* StateTree);

	/**
	 * Stops the currently running state tree and cleanup 
	 */
	UFUNCTION(BlueprintCallable, Category = "StateTree", meta = (DisplayName = "Stop State Tree"))
	void StopStateTree();
protected:
	virtual void BeginPlay() override;

private:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
