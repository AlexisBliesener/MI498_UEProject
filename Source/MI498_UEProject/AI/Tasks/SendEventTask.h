#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MI498_UEProject/AI/EnemyAIController.h"
#include "MI498_UEProject/Characters/Enemies/EnemyBase.h"
#include "SendEventTask.generated.h"


/**
 * StateTree instance data for the Get Random Location task.
 */
USTRUCT(BlueprintType,meta = (DisplayName = "Send Event Task Data"))
struct FSendEventTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyBase> Actor;

	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context, AllowDerivedTypes="true"))
	TObjectPtr<AEnemyAIController> AIController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	FGameplayTag EventTag;
};
USTRUCT(BlueprintType)
struct MI498_UEPROJECT_API FSendEventTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FSendEventTaskInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;
};
