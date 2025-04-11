// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_UpdateState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Character/DFCharacter.h"
#include "Kismet/GameplayStatics.h" 
#include "DFProject.h"

UBTService_UpdateState::UBTService_UpdateState()
	: AttackRange(500.f)
{
	NodeName = TEXT("UpdateState");
	Interval = 0.1f;

	TargetKey = TEXT("TargetActor");
	DistanceKey = TEXT("DistanceToTarget");
	IsInAttackRangeKey = TEXT("IsInAttackRange");
}

void UBTService_UpdateState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
		BlackboardComp->ClearValue(DistanceKey);
		BlackboardComp->SetValueAsBool(IsInAttackRangeKey, false);
		return;
	}

	const float Distance = FVector::Dist(MyCharacter->GetActorLocation(), Target->GetActorLocation());
	BlackboardComp->SetValueAsFloat(DistanceKey, Distance);
	BlackboardComp->SetValueAsBool(IsInAttackRangeKey, Distance <= AttackRange);
}