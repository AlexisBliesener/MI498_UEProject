// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_OtherBrutes.generated.h"

/**
 * This is used to make brute enemies circle the player as each one of them 
 * will take one location around the player 
 */
UCLASS()
class MI498_UEPROJECT_API UEnvQueryContext_OtherBrutes : public UEnvQueryContext
{
	GENERATED_BODY()
public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
