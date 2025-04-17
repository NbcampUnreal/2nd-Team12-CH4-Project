// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_GrabManager.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UBTService_GrabManager : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_GrabManager();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName GrabTargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName CanGrabKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName AILevelKey;

	UPROPERTY(EditAnywhere, Category = "Config")
	float GrabDetectRadius;
};
