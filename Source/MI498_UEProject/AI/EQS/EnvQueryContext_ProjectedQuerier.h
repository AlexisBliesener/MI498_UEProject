// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_ProjectedQuerier.generated.h"

/**
 * A custom environment query context to use in the eqs 
 * it gives the querier location in the fake ship (that has the navmesh)
 * this class is equivalent to the built-in class (EnvQueryContext_Querier)
 */
UCLASS()
class MI498_UEPROJECT_API UEnvQueryContext_ProjectedQuerier : public UEnvQueryContext
{
	GENERATED_BODY()
public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
