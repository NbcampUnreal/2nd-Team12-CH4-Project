// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTService_AttackManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Character/DFCharacter.h"
#include "DFProject.h"

UBTService_AttackManager::UBTService_AttackManager()
{
	NodeName = TEXT("AttackManager");
	Interval = 0.1f;

	TargetKey = TEXT("TargetActor");
	SelectedAttackTypeKey = TEXT("AttackType");

	LastAttackType = 0;
}

void UBTService_AttackManager::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	ADFCharacter* MyCharacter = Cast<ADFCharacter>(AIController->GetPawn());
	if (!MyCharacter) return;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey));
	if (!Target)
	{
		BlackboardComp->SetValueAsEnum(SelectedAttackTypeKey, 0);
		return;
	}

	// 첫 시도는 Headbutt → 실패하면 다음에 Punch 
	if (LastAttackType == 2)
	{
		BlackboardComp->SetValueAsEnum(SelectedAttackTypeKey, 1);
		LastAttackType = 1;
	}
	else
	{
		BlackboardComp->SetValueAsEnum(SelectedAttackTypeKey, 2);
		LastAttackType = 2;
	}
}