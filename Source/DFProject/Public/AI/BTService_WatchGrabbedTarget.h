// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_WatchGrabbedTarget.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UBTService_WatchGrabbedTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_WatchGrabbedTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName GrabTargetActorKey = TEXT("GrabTargetActor");
};
