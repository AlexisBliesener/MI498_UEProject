#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "RandomPointTask.generated.h"



USTRUCT(BlueprintType,meta = (DisplayName = "Get Random Location Data"))
struct FGetRandomLocationTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyBase> Actor;

	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyAIController> AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	float SearchRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Output, meta = (Output))
	FVector RandomLocation = FVector::ZeroVector;
};
USTRUCT(BlueprintType)
struct MI498_UEPROJECT_API FRandomPointTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FGetRandomLocationTaskInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;
};
