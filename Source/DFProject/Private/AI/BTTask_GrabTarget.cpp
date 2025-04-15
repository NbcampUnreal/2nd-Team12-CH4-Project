// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GrabTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/DFCharacter.h"
#include "DFProject.h"

UBTTask_GrabTarget::UBTTask_GrabTarget()
{
	NodeName = TEXT("GrabTarget");
}

EBTNodeResult::Type UBTTask_GrabTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(GrabTargetActorKey));
	if (!Target)
	{
		LOG_WARNING(TEXT("ExecuteTask : No GrabTargetActor"));
		return EBTNodeResult::Failed;
	}

	MyCharacter->Server_StartGrab();

	LOG_WARNING(TEXT("GrabTask: Grab 시도 → 대상 %s"), *Target->GetName());

	return EBTNodeResult::Succeeded;
}
