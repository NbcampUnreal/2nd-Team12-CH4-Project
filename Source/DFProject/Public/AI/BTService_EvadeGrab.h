// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_EvadeGrab.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UBTService_EvadeGrab : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_EvadeGrab();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName EvadeLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName IsEvadingKey;

	UPROPERTY(EditAnywhere, Category = "Evade")
	float EvadeDistance;

	UPROPERTY(EditAnywhere, Category = "Evade")
	float DetectRange;
};
