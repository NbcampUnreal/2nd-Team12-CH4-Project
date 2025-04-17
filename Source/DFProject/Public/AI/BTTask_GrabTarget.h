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

	EBTNodeResult::Type EvaluateAndAttemptGrab(UBehaviorTreeComponent& OwnerComp, class ADFCharacter* MyCharacter, class ADFCharacter* TargetCharacter);

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName GrabTargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Grab")
	float GrabHoldDelay;

	UPROPERTY(EditAnywhere, Category = "Grab")
	float GrabRange;

	FTimerHandle GrabDelayHandle;

	UBehaviorTreeComponent* OwnerCompPtr;
};
