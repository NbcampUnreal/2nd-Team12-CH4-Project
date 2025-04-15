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

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName GrabTargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName DangerDirectionKey;

	UPROPERTY(EditAnywhere, Category = "Throw")
	float ThrowPower;

	
};
