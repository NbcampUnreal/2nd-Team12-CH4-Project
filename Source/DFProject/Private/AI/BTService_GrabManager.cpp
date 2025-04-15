// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_GrabManager.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/DFCharacter.h"
#include "Character/State/CharacterStateManager.h"
#include "Character/State/CharacterStateBase.h" 
#include "Kismet/GameplayStatics.h"
#include "DFProject.h"

UBTService_GrabManager::UBTService_GrabManager()
{
	NodeName = TEXT("GrabManager");

	Interval = 0.3f;
	RandomDeviation = 0.f;

	GrabTargetActorKey = TEXT("GrabTargetActor");
	CanGrabKey = TEXT("bCanGrab");

	GrabDetectRadius = 800.f;
}

void UBTService_GrabManager::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	AActor* ClosestStunned = nullptr;
	float ClosestDistance = FLT_MAX;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADFCharacter::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		ADFCharacter* Enemy = Cast<ADFCharacter>(Actor);
		if (!Enemy)
		{
			continue;
		}

		// 자기 자신은 제외
		if (Enemy == AIPawn)
		{
			continue;
		}

		// 상태 존재 여부 확인
		if (!Enemy->StateManager || !Enemy->StateManager->CurrentState)
		{
			continue;
		}

		// 스턴된 상태 확인
		if (Enemy->StateManager->CurrentState->GetStateType() != ECharacterStateType::Stunned)
		{
			continue;
		}

		const float Distance = FVector::Dist2D(AIPawn->GetActorLocation(), Enemy->GetActorLocation());
		if (Distance <= GrabDetectRadius && Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestStunned = Enemy;
		}
	}

	if (ClosestStunned)
	{
		BlackboardComp->SetValueAsObject(GrabTargetActorKey, ClosestStunned);
		BlackboardComp->SetValueAsBool(CanGrabKey, true);

		LOG_WARNING(TEXT("GrabManager: 스턴된 적 발견 → %s (거리 %.0f)"), *ClosestStunned->GetName(), ClosestDistance);
	}
	else
	{
		BlackboardComp->ClearValue(GrabTargetActorKey);
		BlackboardComp->SetValueAsBool(CanGrabKey, false);

		LOG_WARNING(TEXT("GrabManager: 범위 내 스턴된 적 없음"));
	}
}
