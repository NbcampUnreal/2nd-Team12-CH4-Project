// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ThrowGrabbedTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/DFCharacter.h"
#include "Character/State/CharacterStateManager.h"
#include "Character/State/CharacterStateBase.h"
#include "Character/State/GrabbedState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DFProject.h"
UBTTask_ThrowGrabbedTarget::UBTTask_ThrowGrabbedTarget()
{
	NodeName = TEXT("ThrowGrabbedTarget");
	GrabTargetActorKey = TEXT("GrabTargetActor");
	DangerDirectionKey = TEXT("DangerDirection");
	ThrowPower = 1200.f;
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
		LOG_WARNING(TEXT("ExecuteTask : No BlackboardComponent"));
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(GrabTargetActorKey));
	if (!TargetActor)
	{
		LOG_WARNING(TEXT("ExecuteTask : No TargetActor"));
		return EBTNodeResult::Failed;
	}

	ADFCharacter* Victim = Cast<ADFCharacter>(TargetActor);
	if (!Victim || !Victim->GetCharacterMovement())
	{
		LOG_WARNING(TEXT("ExecuteTask : No Target or Pys"));
		return EBTNodeResult::Failed;
	}

	const FVector Direction = BlackboardComp->GetValueAsVector(DangerDirectionKey);
	if (Direction.IsNearlyZero())
	{
		LOG_WARNING(TEXT("❌ DangerDirection 없음"));
		return EBTNodeResult::Failed;
	}

	// Impulse 적용
		const FVector Impulse = Direction.GetSafeNormal() * ThrowPower;
	Victim->GetMesh()->AddImpulse(Impulse, NAME_None, true);
	LOG_WARNING(TEXT("💥 Impulse 적용: %s"), *Impulse.ToString());

	// 상태가 Idle이면 강제로 Grabbed로 전환 후 해제
	if (MyCharacter->StateManager &&
		MyCharacter->StateManager->CurrentState &&
		MyCharacter->StateManager->CurrentState->GetStateType() == ECharacterStateType::Idle)
	{
		LOG_WARNING(TEXT("⚠️ 상태가 Idle → 강제로 Grabbed 상태 설정 후 해제"));
		MyCharacter->StateManager->SetState(NewObject<UGrabbedState>(MyCharacter));
	}

	// 그랩 해제 실행
	MyCharacter->Server_ReleaseGrab();

	return EBTNodeResult::Succeeded;

}