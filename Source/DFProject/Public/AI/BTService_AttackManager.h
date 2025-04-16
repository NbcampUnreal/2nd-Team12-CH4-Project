// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_AttackManager.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UBTService_AttackManager : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_AttackManager();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	FName TargetKey; 
	FName CanAttackKey; 
};
