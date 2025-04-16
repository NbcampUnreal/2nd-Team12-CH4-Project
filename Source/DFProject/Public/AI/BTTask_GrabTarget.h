// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GrabTarget.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UBTTask_GrabTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GrabTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** 블랙보드 키: Grab 대상 */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName GrabTargetActorKey;

private:
	EBTNodeResult::Type EvaluateAndAttemptGrab(class ADFCharacter* MyCharacter, class ADFCharacter* TargetCharacter);
};
