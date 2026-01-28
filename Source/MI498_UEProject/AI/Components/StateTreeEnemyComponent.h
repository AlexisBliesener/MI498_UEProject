// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeAIComponent.h"
#include "StateTreeEnemyComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent, DisplayName = "State Tree AI Component"))
class MI498_UEPROJECT_API UStateTreeEnemyComponent : public UStateTreeAIComponent
{
	GENERATED_BODY()

public:

	UStateTreeEnemyComponent();
	
	UFUNCTION(BlueprintCallable, Category = "StateTree", meta = (AutoCreateRefTerm = "StateTreeReference", DisplayName = "Start State Tree"))
	void StartStateTree(UStateTree* StateTree);

	UFUNCTION(BlueprintCallable, Category = "StateTree", meta = (DisplayName = "Stop State Tree"))
	void StopStateTree();
protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
