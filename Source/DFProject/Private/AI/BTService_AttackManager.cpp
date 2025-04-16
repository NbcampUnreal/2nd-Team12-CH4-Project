// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTService_AttackManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/DFAIController.h"
#include "Character/DFCharacter.h"
#include "DFProject.h"

UBTService_AttackManager::UBTService_AttackManager()
{
	NodeName = TEXT("AttackManager");
	Interval = 0.4f;

	TargetKey = TEXT("TargetActor");
	CanAttackKey = TEXT("CanAttack");
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
	BlackboardComp->SetValueAsBool(CanAttackKey, Target != nullptr);

	LOG_WARNING(TEXT("[AttackManager] Target %s → CanAttack = %s"),
		Target ? *Target->GetName() : TEXT("None"),
		Target ? TEXT("true") : TEXT("false"));
}