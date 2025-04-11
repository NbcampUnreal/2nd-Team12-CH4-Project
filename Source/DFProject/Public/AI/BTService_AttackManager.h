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

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName TargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName SelectedAttackTypeKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float PunchCooldown;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float HeadbuttCooldown;

	UPROPERTY(EditAnywhere, Category = "AttackRange")
	float PunchRange;

	UPROPERTY(EditAnywhere, Category = "AttackRange")
	float HeadbuttRange;

private:
	float LastPunchTime;
	float LastHeadbuttTime;
	
};
