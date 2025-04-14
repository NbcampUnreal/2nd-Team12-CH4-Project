// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_AvoidDanger.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Level/DFDeadZoneComponent.h"
#include "DFProject.h"
#include "DrawDebugHelpers.h"


UBTService_AvoidDanger::UBTService_AvoidDanger()
{
	NodeName = TEXT("AvoidDanger");

	AvoidLocationKey = TEXT("AvoidLocation");
	IsNearDangerKey = TEXT("IsNearDanger");

	Interval = 0.5f;
}

void UBTService_AvoidDanger::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		LOG_WARNING(TEXT("TickNode: No AIController"));
		return;
	}

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		LOG_WARNING(TEXT("TickNode: No Pawn"));
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		LOG_WARNING(TEXT("TickNode: No BlackboardComponent"));
		return;
	}

	const FVector MyLocation = AIPawn->GetActorLocation();

	// ✅ Danger 상태 중일 때만 해제 조건 검사
	const bool bCurrentlyInDanger = BlackboardComp->GetValueAsBool(IsNearDangerKey);
	if (bCurrentlyInDanger)
	{
		const FVector AvoidLoc = BlackboardComp->GetValueAsVector(AvoidLocationKey);
		const float DistanceToAvoidTarget = FVector::Dist2D(MyLocation, AvoidLoc);
		const float EscapeDistanceThreshold = 400.f;

		if (!AvoidLoc.IsNearlyZero() && DistanceToAvoidTarget > EscapeDistanceThreshold)
		{
			LOG_WARNING(TEXT("✅ 충분히 회피 성공 → Danger 해제"));
			BlackboardComp->ClearValue(AvoidLocationKey);
			BlackboardComp->SetValueAsBool(IsNearDangerKey, false);
			return;
		}
	}

	// ✅ Danger 감지 시작
	FVector DangerDirection = FVector::ZeroVector;
	bool bDangerDetected = false;

	// NavMesh 경계 감지 방향
	FVector NavEdgeDir = FVector::ZeroVector;
	if (IsNearNavEdge(MyLocation, NavEdgeDir))
	{
		DangerDirection += NavEdgeDir;
		bDangerDetected = true;
	}

	// DeadZone 감지 방향
	FVector DeadZoneDir = FVector::ZeroVector;
	if (IsNearDeadZones(MyLocation, DeadZoneDir))
	{
		DangerDirection += DeadZoneDir;
		bDangerDetected = true;
	}

	// ✅ Danger 감지됨 → 회피 위치 설정
	if (bDangerDetected && !DangerDirection.IsNearlyZero())
	{
		const FVector AvoidDir = -DangerDirection.GetSafeNormal();
		const FVector RawAvoidLoc = MyLocation + AvoidDir * AvoidDistance;

		if (bDebugLog)
		{
			LOG_WARNING(TEXT("▶ DangerDirection: %s"), *DangerDirection.ToString());
			LOG_WARNING(TEXT("▶ RawAvoidLoc (Before Projection): %s"), *RawAvoidLoc.ToString());
			DrawDebugSphere(GetWorld(), RawAvoidLoc, 40.f, 12, FColor::Red, false, 1.0f);
		}

		FNavLocation Projected;
		if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
		{
			if (NavSys->ProjectPointToNavigation(RawAvoidLoc, Projected, FVector(100.f)))
			{
				BlackboardComp->SetValueAsVector(AvoidLocationKey, Projected.Location);
				LOG_WARNING(TEXT("✅ Projected AvoidLocation: %s"), *Projected.Location.ToString());
				DrawDebugSphere(GetWorld(), Projected.Location, 30.f, 12, FColor::Green, false, 1.0f);
			}
			else
			{
				// fallback
				const FVector SearchOrigin = MyLocation + AIPawn->GetActorForwardVector() * 300.f;
				FNavLocation RandomSafeLoc;
				if (NavSys->GetRandomReachablePointInRadius(SearchOrigin, 1000.f, RandomSafeLoc))
				{
					BlackboardComp->SetValueAsVector(AvoidLocationKey, RandomSafeLoc.Location);
					LOG_WARNING(TEXT("✅ Fallback SafeLocation: %s"), *RandomSafeLoc.Location.ToString());
					DrawDebugSphere(GetWorld(), RandomSafeLoc.Location, 30.f, 12, FColor::Blue, false, 1.0f);
				}
				else
				{
					LOG_WARNING(TEXT("❌ Fallback 탐색 실패 → Danger 해제"));
					BlackboardComp->ClearValue(AvoidLocationKey);
					bDangerDetected = false;
				}
			}
		}
	}
	else
	{
		BlackboardComp->ClearValue(AvoidLocationKey);
	}

	// ✅ Danger 상태 최종 반영
	BlackboardComp->SetValueAsBool(IsNearDangerKey, bDangerDetected);
}

bool UBTService_AvoidDanger::IsNearNavEdge(const FVector& Location, FVector& OutDirection) const
{
	OutDirection = FVector::ZeroVector;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return false;

	const FVector DetectDirection = FVector(1, 0, -1).GetSafeNormal(); // 오른쪽 아래
	const float DetectDistance = 800.f; // 멀리 쏴본다
	const FVector TestPoint = Location + DetectDirection * DetectDistance;

	FNavLocation Projected;
	bool bOnNav = NavSys->ProjectPointToNavigation(TestPoint, Projected, FVector(200.f)); // 큰 Extent

	if (!bOnNav)
	{
		OutDirection = FVector(-DetectDirection.X, -DetectDirection.Y, 0.f).GetSafeNormal(); // XY 반대방향
		LOG_WARNING(TEXT("✅ 경계 감지됨 (검사 좌표: %s) → 회피 방향: %s"), *TestPoint.ToString(), *OutDirection.ToString());
		DrawDebugSphere(GetWorld(), TestPoint, 30.f, 12, FColor::Red, false, 2.f);
		return true;
	}

	DrawDebugSphere(GetWorld(), TestPoint, 30.f, 12, FColor::Green, false, 2.f);
	return false;
}

bool UBTService_AvoidDanger::IsNearDeadZones(const FVector& Location, FVector& OutDirection) const
{
	OutDirection = FVector::ZeroVector;

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("DFDeadZoneComponent"), Found);

	bool bDetected = false;

	for (AActor* Zone : Found)
	{
		if (!Zone) continue;

		const FVector ToDanger = Zone->GetActorLocation() - Location;
		if (ToDanger.Size() <= DetectRadius)
		{
			OutDirection += ToDanger;
			bDetected = true;
		}
	}

	return bDetected;
}