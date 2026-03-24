// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/StateTreeMoveToTask.h"
#include "MoveToCustomTask.generated.h"

/**
 * This task is inherited from unreal's mvoe to task
 * it just follows our fake ship thing because the base class doesn't support moving to a target that updates continuously
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Move To Custom", Category = "AI|Action"))
struct MI498_UEPROJECT_API FMoveToCustomTask : public FStateTreeMoveToTask
{
public:
	GENERATED_BODY()
	
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
};
