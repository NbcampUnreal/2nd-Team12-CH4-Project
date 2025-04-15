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

	// ✅ Danger 해제 거리 조정: 500
	const float DangerEscapeThreshold = 500.f;
	if (BlackboardComp->GetValueAsBool(IsNearDangerKey))
	{
		const FVector AvoidLoc = BlackboardComp->GetValueAsVector(AvoidLocationKey);
		if (!AvoidLoc.IsNearlyZero())
		{
			const float Distance = FVector::Dist2D(MyLocation, AvoidLoc);
			if (Distance > DangerEscapeThreshold)
			{
				LOG_WARNING(TEXT("✅ 충분히 회피 성공 → Danger 해제"));
				BlackboardComp->ClearValue(AvoidLocationKey);
				BlackboardComp->SetValueAsBool(IsNearDangerKey, false);
				return;
			}
		}
	}

	// ✅ Danger 감지
	FVector DangerDirection = FVector::ZeroVector;
	bool bDangerDetected = false;

	FVector NavEdgeDir = FVector::ZeroVector;
	if (IsNearNavEdge(MyLocation, NavEdgeDir))
	{
		DangerDirection += NavEdgeDir;
		bDangerDetected = true;
	}

	FVector DeadZoneDir = FVector::ZeroVector;
	if (IsNearDeadZones(MyLocation, DeadZoneDir))
	{
		DangerDirection += DeadZoneDir;
		bDangerDetected = true;
	}

	if (bDangerDetected && !DangerDirection.IsNearlyZero())
	{
		const FVector AvoidDir = DangerDirection.GetSafeNormal2D();
		const FVector RawAvoidLoc = MyLocation + AvoidDir * AvoidDistance * 2.f; // ✅ 더 가까이 회피

		if (bDebugLog)
		{
			LOG_WARNING(TEXT("▶ DangerDirection: %s"), *DangerDirection.ToString());
			LOG_WARNING(TEXT("▶ RawAvoidLoc (Before Projection): %s"), *RawAvoidLoc.ToString());
			DrawDebugSphere(GetWorld(), RawAvoidLoc, 40.f, 12, FColor::Red, false, 1.0f);
		}

		FNavLocation Projected;
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

		if (NavSys && NavSys->ProjectPointToNavigation(RawAvoidLoc, Projected, FVector(100.f)))
		{
			BlackboardComp->SetValueAsVector(AvoidLocationKey, Projected.Location);
			LOG_WARNING(TEXT("✅ Projected AvoidLocation: %s"), *Projected.Location.ToString());
			DrawDebugSphere(GetWorld(), Projected.Location, 30.f, 12, FColor::Green, false, 1.0f);
		}
		else
		{
			const FVector SearchOrigin = MyLocation + AvoidDir * 300.f;
			FNavLocation RandomSafeLoc;

			if (NavSys && NavSys->GetRandomReachablePointInRadius(SearchOrigin, 1000.f, RandomSafeLoc))
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
	else
	{
		BlackboardComp->ClearValue(AvoidLocationKey);
	}

	// ✅ 최종 Danger 상태 반영
	BlackboardComp->SetValueAsBool(IsNearDangerKey, bDangerDetected);
}

bool UBTService_AvoidDanger::IsNearNavEdge(const FVector& Location, FVector& OutDirection) const
{
	OutDirection = FVector::ZeroVector;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return false;

	const TArray<FVector> Directions = {
		{1, 0, -0.3f}, {-1, 0, -0.3f},
		{0, 1, -0.3f}, {0, -1, -0.3f},
		{1, 1, -0.3f}, {-1, 1, -0.3f},
		{1, -1, -0.3f}, {-1, -1, -0.3f},
	};

	const float DetectDistance = 600.f;

	for (const FVector& Dir : Directions)
	{
		const FVector TestPoint = Location + Dir.GetSafeNormal() * DetectDistance;
		FNavLocation Projected;

		bool bOnNav = NavSys->ProjectPointToNavigation(TestPoint, Projected, FVector(200.f));
		if (!bOnNav)
		{
			OutDirection = FVector(-Dir.X, -Dir.Y, 0.f).GetSafeNormal();
			LOG_WARNING(TEXT("✅ NavMesh 경계 감지 방향: %s → 회피 방향: %s"), *Dir.ToString(), *OutDirection.ToString());
			DrawDebugSphere(GetWorld(), TestPoint, 30.f, 12, FColor::Red, false, 2.0f);
			return true;
		}
	}

	return false;
}

bool UBTService_AvoidDanger::IsNearDeadZones(const FVector& Location, FVector& OutDirection) const
{
	OutDirection = FVector::ZeroVector;

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("DFDeadZoneComponent"), Found);

	LOG_WARNING(TEXT("🟡 DeadZone 찾은 개수: %d"), Found.Num());

	bool bDetected = false;
	float ClosestDist = FLT_MAX;
	FVector ClosestDir = FVector::ZeroVector;

	for (AActor* Zone : Found)
	{
		if (!Zone) continue;

		const float Dist = FVector::Dist2D(Zone->GetActorLocation(), Location);
		if (Dist <= DetectRadius && Dist < ClosestDist)
		{
			ClosestDist = Dist;
			ClosestDir = Zone->GetActorLocation() - Location;
			bDetected = true;

			if (bDebugLog)
			{
				DrawDebugSphere(GetWorld(), Zone->GetActorLocation(), 50.f, 12, FColor::Purple, false, 2.0f);
				LOG_WARNING(TEXT("🟣 DeadZone 감지 위치: %s | 거리: %f"), *Zone->GetActorLocation().ToString(), Dist);
			}
		}
	}

	OutDirection = ClosestDir;
	return bDetected;
}