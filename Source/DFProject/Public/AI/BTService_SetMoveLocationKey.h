// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SetMoveLocationKey.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UBTService_SetMoveLocationKey : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_SetMoveLocationKey();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName AvoidLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName EvadeLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName MoveLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName EvadeFlagKey;
};
