// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_SetMoveLocationKey.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DFProject.h"

UBTService_SetMoveLocationKey::UBTService_SetMoveLocationKey()
{
	NodeName = TEXT("Set MoveLocationKey");
	Interval = 0.1f;
	TargetActorKey = TEXT("TargetActor");
	AvoidLocationKey = TEXT("AvoidLocation");
	EvadeLocationKey = TEXT("EvadeLocation");
	MoveLocationKey = TEXT("MoveLocation");
	EvadeFlagKey = TEXT("IsEvadingGrab");

}

void UBTService_SetMoveLocationKey::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	FName FinalTargetKey = TargetActorKey;

	const bool bEvadeValid = BlackboardComp->IsVectorValueSet(EvadeLocationKey);
	const bool bAvoidValid = BlackboardComp->IsVectorValueSet(AvoidLocationKey);
	const bool bEvading = BlackboardComp->GetValueAsBool(EvadeFlagKey);

	LOG_WARNING(TEXT("BTService_SetMoveLocationKey: bEvade=%d, bEvading=%d, bAvoid=%d"), bEvadeValid, bEvading, bAvoidValid);

	if (bEvadeValid && bEvading)
	{
		FinalTargetKey = EvadeLocationKey;
	}
	else if (bAvoidValid)
	{
		FinalTargetKey = AvoidLocationKey;
	}
	else
	{
		FinalTargetKey = TargetActorKey;
	}

	BlackboardComp->SetValueAsName(MoveLocationKey, FinalTargetKey);

	LOG_WARNING(TEXT("SetMoveLocationService: %s → %s"), *MoveLocationKey.ToString(), *FinalTargetKey.ToString());
}