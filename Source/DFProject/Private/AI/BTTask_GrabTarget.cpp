// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GrabTarget.h"
#include "AI/DFAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/DFCharacter.h"
#include "Character/State/CharacterStateManager.h"
#include "Character/State/CharacterStateBase.h" 
#include "DFProject.h"

UBTTask_GrabTarget::UBTTask_GrabTarget()
{
	NodeName = TEXT("GrabTarget");
	GrabTargetActorKey = TEXT("GrabTargetActor");
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

	return EvaluateAndAttemptGrab(MyCharacter, TargetCharacter);
}

EBTNodeResult::Type UBTTask_GrabTarget::EvaluateAndAttemptGrab(ADFCharacter* MyCharacter, ADFCharacter* TargetCharacter)
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

	if (TargetState != ECharacterStateType::Stunned && MyState != ECharacterStateType::Stunned)
	{
		LOG_WARNING(TEXT("GrabTask: 대상이 스턴 상태가 아님 (현재 상태: %d) → 실패"), static_cast<int32>(TargetState));
		MyCharacter->Server_ReleaseGrab();
		MyCharacter->Server_StopGrab();
		return EBTNodeResult::Failed;
	}

	// Grab 실행
	MyCharacter->Server_StartGrab();
	LOG_WARNING(TEXT("GrabTask: Grab 시도 성공 → 대상 %s"), *TargetCharacter->GetName());
	return EBTNodeResult::Succeeded;
}