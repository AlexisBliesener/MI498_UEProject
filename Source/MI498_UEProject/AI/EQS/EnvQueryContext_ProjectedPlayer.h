// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_ProjectedPlayer.generated.h"

/**
 * A custom environment query context to use in the eqs 
 * it gives the player location in the fake ship (that has the navmesh) 
 */
UCLASS()
class MI498_UEPROJECT_API UEnvQueryContext_ProjectedPlayer : public UEnvQueryContext
{
	GENERATED_BODY()
public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
