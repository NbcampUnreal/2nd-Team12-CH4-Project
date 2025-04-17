// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_AvoidDanger.generated.h"

/**
 * 
 * 
 */
UCLASS()
class DFPROJECT_API UBTService_AvoidDanger : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_AvoidDanger();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool IsNearNavEdge(const FVector& Location, FVector& OutDirection) const;

	bool IsNearDeadZones(const FVector& Location, FVector& OutDirection, FVector& OutNearestZoneLoc) const;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName AvoidLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName IsNearDangerKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName DangerDirectionKey;

	UPROPERTY(EditAnywhere, Category = "Config")
	float AvoidDistance = 600.f;

	UPROPERTY(EditAnywhere, Category = "Config")
	float DetectRadius = 600.f;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebugLog = true;
};
