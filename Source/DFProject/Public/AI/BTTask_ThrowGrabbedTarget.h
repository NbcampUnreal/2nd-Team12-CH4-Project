// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ThrowGrabbedTarget.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UBTTask_ThrowGrabbedTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ThrowGrabbedTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// 블랙보드 키
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName GrabTargetActorKey;

	// 던지는 힘
	UPROPERTY(EditAnywhere, Category = "Throw")
	float ThrowPower;

	// ✅ 딜레이 시간 (초)
	UPROPERTY(EditAnywhere, Category = "Throw")
	float ThrowDelay;

	FTimerHandle DelayHandle;

	// 던지기 실행 함수
private:
	EBTNodeResult::Type EvaluateAndAttemptThrow(class ADFCharacter* MyCharacter, class ADFCharacter* TargetCharacter);
};
