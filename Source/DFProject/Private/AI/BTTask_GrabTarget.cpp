// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GrabTarget.h"
#include "AI/DFAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/DFCharacter.h"
#include "Character/State/CharacterStateManager.h"
#include "Character/State/CharacterStateBase.h" 
#include "TimerManager.h"
#include "DFProject.h"

UBTTask_GrabTarget::UBTTask_GrabTarget()
{
	NodeName = TEXT("GrabTarget");

	GrabTargetActorKey = TEXT("GrabTargetActor");
	GrabHoldDelay = 1.0f; 
	GrabRange = 200.f; 
}

EBTNodeResult::Type UBTTask_GrabTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADFAIController* AIController = Cast<ADFAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		LOG_ERROR(TEXT("DFAIController not found"));
		return EBTNodeResult::Failed;
	}

	ADFCharacter* MyCharacter = Cast<ADFCharacter>(AIController->GetPawn());
	if (!MyCharacter)
	{
		LOG_WARNING(TEXT("ExecuteTask : No Character"));
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		LOG_WARNING(TEXT("ExecuteTask : No BlackboardComponent"));
		return EBTNodeResult::Failed;
	}
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(GrabTargetActorKey));
	ADFCharacter* TargetCharacter = Cast<ADFCharacter>(Target);

	return EvaluateAndAttemptGrab(OwnerComp, MyCharacter, TargetCharacter);
}

EBTNodeResult::Type UBTTask_GrabTarget::EvaluateAndAttemptGrab(UBehaviorTreeComponent& OwnerComp, ADFCharacter* MyCharacter, ADFCharacter* TargetCharacter)
{
	if (!MyCharacter)
	{
		LOG_WARNING(TEXT("GrabTask: MyCharacter 없음"));
		return EBTNodeResult::Failed;
	}

	if (!TargetCharacter)
	{
		LOG_WARNING(TEXT("GrabTask: GrabTargetCharacter 없음"));
		return EBTNodeResult::Failed;
	}

	if (!TargetCharacter->StateManager || !TargetCharacter->StateManager->CurrentState)
	{
		LOG_WARNING(TEXT("GrabTask: 대상 상태 정보 없음"));
		return EBTNodeResult::Failed;
	}

	const ECharacterStateType TargetState = TargetCharacter->StateManager->CurrentState->GetStateType();
	const ECharacterStateType MyState = MyCharacter->StateManager->CurrentState->GetStateType();

	if (TargetState != ECharacterStateType::Stunned || MyState != ECharacterStateType::Idle)
	{
		LOG_WARNING(TEXT("GrabTask: 유효하지 않은 상태 → 대상: %d, 나: %d"), static_cast<int32>(TargetState), static_cast<int32>(MyState));
		MyCharacter->Server_ReleaseGrab();
		MyCharacter->Server_StopGrab();
		return EBTNodeResult::Failed;
	}

	const float Distance = FVector::Dist2D(MyCharacter->GetActorLocation(), TargetCharacter->GetActorLocation());
	if (Distance > GrabRange)
	{
		LOG_WARNING(TEXT("GrabTask: 거리 초과 → %f / 최대 %f"), Distance, GrabRange);
		return EBTNodeResult::Failed;
	}

	MyCharacter->Server_StartGrab();
	LOG_WARNING(TEXT("GrabTask: Grab 시도 성공 → 대상 %s"), *TargetCharacter->GetName());

	FTimerDelegate FinishDelegate;
	OwnerCompPtr = &OwnerComp; 

	FinishDelegate.BindLambda([=, this]()
		{
			FinishLatentTask(*OwnerCompPtr, EBTNodeResult::Succeeded);
		});

	MyCharacter->GetWorld()->GetTimerManager().SetTimer(
		GrabDelayHandle, FinishDelegate, GrabHoldDelay, false);

	return EBTNodeResult::InProgress;
}