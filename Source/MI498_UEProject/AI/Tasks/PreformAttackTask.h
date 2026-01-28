#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "PreformAttackTask.generated.h"



USTRUCT(BlueprintType, meta = (DisplayName = "Get Random Location Data"))
struct FPreformAttackTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyBase> Actor;

	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyAIController> AIController;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<AActor> Target = nullptr;
};
USTRUCT(BlueprintType)
struct MI498_UEPROJECT_API FPreformAttackTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FPreformAttackTaskInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;
};
