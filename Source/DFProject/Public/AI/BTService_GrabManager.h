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

	/** 블랙보드: 그랩 가능한 대상 (스턴된 적) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName GrabTargetActorKey;

	/** 블랙보드: 그랩 가능 여부 */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName CanGrabKey;

	/** 그랩 감지 범위 (기본 800) */
	UPROPERTY(EditAnywhere, Category = "Config")
	float GrabDetectRadius;

};
