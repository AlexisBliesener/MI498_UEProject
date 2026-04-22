// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_PlayerCamera.generated.h"

/**
 * Give a context from the player camera for EQS 
 */
UCLASS()
class MI498_UEPROJECT_API UEnvQueryContext_PlayerCamera : public UEnvQueryContext
{
	GENERATED_BODY()
public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
