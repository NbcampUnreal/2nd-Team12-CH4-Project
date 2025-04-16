// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ThrowGrabbedTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/DFAIController.h"
#include "Character/DFCharacter.h"
#include "Character/State/CharacterStateManager.h"
#include "Character/State/CharacterStateBase.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "DFProject.h"

UBTTask_ThrowGrabbedTarget::UBTTask_ThrowGrabbedTarget()
{
	NodeName = TEXT("ThrowGrabbedTarget");

	GrabTargetActorKey = TEXT("GrabTargetActor");

	ThrowPower = 1000.f;
	ThrowDelay = 1.0f;

	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_ThrowGrabbedTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		LOG_WARNING(TEXT("ExecuteTask : No AIController"));
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
		LOG_WARNING(TEXT("ExecuteTask : BlackboardComponent not found"));
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(GrabTargetActorKey));
	if (!TargetActor)
	{
		LOG_WARNING(TEXT("ExecuteTask : No Grabbed Target Actor"));
		return EBTNodeResult::Failed;
	}

	return EvaluateAndAttemptThrow(MyCharacter, MyCharacter); 
}

EBTNodeResult::Type UBTTask_ThrowGrabbedTarget::EvaluateAndAttemptThrow(ADFCharacter* MyCharacter, ADFCharacter* TargetCharacter)
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
	if (TargetState != ECharacterStateType::Stunned)
	{
		//LOG_WARNING(TEXT("ThrowTask: 대상이 스턴 상태 또는 그랩 상태 아님 (현재 상태: %d) → 실패"), static_cast<int32>(TargetState));
		MyCharacter->Server_ReleaseGrab(); 
		return EBTNodeResult::Failed;
	}

	MyCharacter->Server_ReleaseGrab();  

	MyCharacter->Jump();

	return EBTNodeResult::Succeeded;
}