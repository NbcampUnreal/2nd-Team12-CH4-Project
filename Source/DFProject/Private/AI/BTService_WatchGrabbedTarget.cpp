// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_WatchGrabbedTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/DFCharacter.h"
#include "Character/State/CharacterStateManager.h"
#include "Character/State/CharacterStateBase.h"
#include "DFProject.h"


UBTService_WatchGrabbedTarget::UBTService_WatchGrabbedTarget()
{
	NodeName = TEXT("WatchGrabbedTarget");

	Interval = 0.3f;
	RandomDeviation = 0.f;

	GrabTargetActorKey = TEXT("GrabTargetActor");
}

void UBTService_WatchGrabbedTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		LOG_WARNING(TEXT("TickNode: No AIController"));
		return;
	}

	ADFCharacter* MyCharacter = Cast<ADFCharacter>(AIController->GetPawn());
	if (!MyCharacter)
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

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(GrabTargetActorKey));
	if (!TargetActor)
	{
		LOG_WARNING(TEXT("TickNode: No TargetActor"));
		return;
	}

	ADFCharacter* Victim = Cast<ADFCharacter>(TargetActor);
	if (!Victim || !Victim->StateManager || !Victim->StateManager->CurrentState)
	{
		LOG_WARNING(TEXT("WatchGrabbedTarget: ❌ GrabTarget 유효하지 않음 또는 상태 없음"));
		return;
	}

	// 상태 타입 확인
	ECharacterStateType State = Victim->StateManager->CurrentState->GetStateType();

	// 스턴도 아니고, Grabbed도 아니라면 → 손에서 해제
	if (State != ECharacterStateType::Stunned && State != ECharacterStateType::Grabbed)
	{
		MyCharacter->Server_ReleaseGrab(); // Server_StopGrab은 생략: Release 안에 포함된 경우
		LOG_WARNING(TEXT("✅ 대상 상태 = %d → 스턴 종료 감지, 손에서 해제 시도"), (int32)State);
	}
}