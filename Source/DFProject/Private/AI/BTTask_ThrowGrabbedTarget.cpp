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
		MyCharacter->Server_ReleaseGrab(); 
		return EBTNodeResult::Failed;
	}
	
	MyCharacter->Jump();

	MyCharacter->Server_ReleaseGrab();  


	const FVector ThrowDirection = (TargetCharacter->GetActorLocation() - MyCharacter->GetActorLocation()).GetSafeNormal();
	if (UPrimitiveComponent* Mesh = TargetCharacter->GetMesh())
	{
		Mesh->AddImpulse(ThrowDirection * ThrowPower, NAME_None, true);
		LOG_WARNING(TEXT("ThrowTask: 던지기 성공 (Impulse 적용)"));
	}
	else
	{
		LOG_WARNING(TEXT("ThrowTask: 대상 Mesh 없음 → 던지기 실패"));
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}