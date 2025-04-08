// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateState.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UBTService_UpdateState : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateState();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FName DistanceKey;    

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FName IsInAttackRangeKey;
};
