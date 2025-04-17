// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_AvoidDanger.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/DFAIController.h"
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
	DangerDirectionKey = TEXT("DangerDirection");

	Interval = 0.5f;
}

void UBTService_AvoidDanger::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ADFAIController* AIController = Cast<ADFAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
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

	const float DangerEscapeThreshold = 500.f;
	if (BlackboardComp->GetValueAsBool(IsNearDangerKey))
	{
		const FVector AvoidLoc = BlackboardComp->GetValueAsVector(AvoidLocationKey);
		if (!AvoidLoc.IsNearlyZero())
		{
			const float Distance = FVector::Dist2D(MyLocation, AvoidLoc);
			if (Distance > DangerEscapeThreshold)
			{
				LOG_WARNING(TEXT("Avoid OO -> Danger Clear"));
				BlackboardComp->ClearValue(AvoidLocationKey);
				BlackboardComp->SetValueAsBool(IsNearDangerKey, false);
				return;
			}
		}
	}

	FVector DangerDirection = FVector::ZeroVector;
	bool bDangerDetected = false;

	FVector NavEdgeDir = FVector::ZeroVector;
	if (IsNearNavEdge(MyLocation, NavEdgeDir))
	{
		DangerDirection += NavEdgeDir;
		bDangerDetected = true;
	}

	FVector DeadZoneDir = FVector::ZeroVector;
	FVector NearestDeadZoneLoc = FVector::ZeroVector;
	if (IsNearDeadZones(MyLocation, DeadZoneDir, NearestDeadZoneLoc))
	{
		DangerDirection += DeadZoneDir;
		bDangerDetected = true;
	}

	if (bDangerDetected && !DangerDirection.IsNearlyZero())
	{
		const FVector AvoidDir = -DangerDirection.GetSafeNormal2D();

		const float DeadZoneRadius = 100.f;
		const float SafeMargin = 200.f;
		const float FinalAvoidDistance = 500.f; 

		const FVector RawAvoidLoc = MyLocation + AvoidDir * FinalAvoidDistance;

		if (bDebugLog)
		{
			LOG_WARNING(TEXT(" DangerDirection : %s"), *DangerDirection.ToString());
			LOG_WARNING(TEXT(" AvoidDirection : %s"), *AvoidDir.ToString());
			LOG_WARNING(TEXT(" RawAvoidLoc (Before Projection): %s"), *RawAvoidLoc.ToString());
			//DrawDebugSphere(GetWorld(), RawAvoidLoc, 40.f, 12, FColor::Red, false, 1.0f);
		}

		FNavLocation Projected;
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

		const float MinAvoidDistanceFromDeadZone = 300.f;

		bool bTooCloseToDeadZone =
			!NearestDeadZoneLoc.IsNearlyZero() &&
			FVector::Dist2D(RawAvoidLoc, NearestDeadZoneLoc) < MinAvoidDistanceFromDeadZone;

		if (NavSys && !bTooCloseToDeadZone &&
			NavSys->ProjectPointToNavigation(RawAvoidLoc, Projected, FVector(100.f)))
		{
			BlackboardComp->SetValueAsVector(AvoidLocationKey, Projected.Location);
			LOG_WARNING(TEXT("Projected AvoidLocation: %s"), *Projected.Location.ToString());
			//DrawDebugSphere(GetWorld(), Projected.Location, 30.f, 12, FColor::Green, false, 1.0f);
		}
		else
		{
			const FVector SearchOrigin = MyLocation + AvoidDir * 300.f;
			FNavLocation RandomSafeLoc;

			if (NavSys && NavSys->GetRandomReachablePointInRadius(SearchOrigin, 1000.f, RandomSafeLoc))
			{
				BlackboardComp->SetValueAsVector(AvoidLocationKey, RandomSafeLoc.Location);
				LOG_WARNING(TEXT("Fallback SafeLocation: %s"), *RandomSafeLoc.Location.ToString());
				//DrawDebugSphere(GetWorld(), RandomSafeLoc.Location, 30.f, 12, FColor::Blue, false, 1.0f);
			}
			else
			{
				LOG_WARNING(TEXT("Fallback XX -> Danger Clear"));
				BlackboardComp->ClearValue(AvoidLocationKey);
				bDangerDetected = false;
			}
		}
		BlackboardComp->SetValueAsVector(DangerDirectionKey, DangerDirection.GetSafeNormal2D());
	}
	else
	{
		BlackboardComp->ClearValue(AvoidLocationKey);
		BlackboardComp->ClearValue(DangerDirectionKey);
	}

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
			LOG_WARNING(TEXT("Search NavMesh : %s -> Avoiddirction: %s"), *Dir.ToString(), *OutDirection.ToString());
			//DrawDebugSphere(GetWorld(), TestPoint, 30.f, 12, FColor::Red, false, 2.0f);
			return true;
		}
	}

	return false;
}

bool UBTService_AvoidDanger::IsNearDeadZones(const FVector& Location, FVector& OutDirection, FVector& OutNearestZoneLoc) const
{
	OutDirection = FVector::ZeroVector;
	OutNearestZoneLoc = FVector::ZeroVector;

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("DFDeadZoneComponent"), Found);

	bool bDetected = false;
	float ClosestDist = FLT_MAX;
	FVector ClosestDir = FVector::ZeroVector;
	FVector ClosestLoc = FVector::ZeroVector;

	for (AActor* Zone : Found)
	{
		if (!Zone) continue;

		const FVector ZoneLoc = Zone->GetActorLocation();
		const float DeadZoneZOffset = 300.f;
		const FVector AdjustedZoneLoc = ZoneLoc + FVector(0.f, 0.f, DeadZoneZOffset);
		const float Dist = FVector::Dist(AdjustedZoneLoc, Location);

		if (Dist <= DetectRadius && Dist < ClosestDist)
		{
			ClosestDist = Dist;

			FVector ZoneFlatLoc = FVector(ZoneLoc.X, ZoneLoc.Y, Location.Z);
			ClosestDir = ZoneFlatLoc - Location;

			ClosestLoc = ZoneLoc;
			bDetected = true;

			if (bDebugLog)
			{
				//DrawDebugSphere(GetWorld(), ZoneLoc, 50.f, 12, FColor::Purple, false, 2.0f);
				LOG_WARNING(TEXT("DeadZone Location : %s / Distance : %f"), *ZoneLoc.ToString(), Dist);
			}
		}
	}

	OutDirection = ClosestDir;
	OutNearestZoneLoc = ClosestLoc;
	return bDetected;
}