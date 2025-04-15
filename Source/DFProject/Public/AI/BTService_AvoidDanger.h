// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_AvoidDanger.generated.h"

/**
 * 위험 감지 시 EQS를 통해 회피 지점을 계산하고,
 * 블랙보드 키에 설정하는 서비스입니다.
 */
UCLASS()
class DFPROJECT_API UBTService_AvoidDanger : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_AvoidDanger();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// NavMesh 경계 감지
	bool IsNearNavEdge(const FVector& Location, FVector& OutDirection) const;

	// DeadZone 탐지
	bool IsNearDeadZones(const FVector& Location, FVector& OutDirection, FVector& OutNearestZoneLoc) const;

protected:
	/** 블랙보드 키 */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName AvoidLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName IsNearDangerKey;

	/** 던기기용 **/
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName DangerDirectionKey;

	/** 위험 지점으로부터 피할 거리 */
	UPROPERTY(EditAnywhere, Category = "Config")
	float AvoidDistance = 600.f;

	/** 위험 지점 탐지 반경 */
	UPROPERTY(EditAnywhere, Category = "Config")
	float DetectRadius = 600.f;

	/** 디버깅 */
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebugLog = true;
};
