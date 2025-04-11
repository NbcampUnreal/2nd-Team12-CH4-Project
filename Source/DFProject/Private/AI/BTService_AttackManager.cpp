// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTService_AttackManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Character/DFCharacter.h"
#include "DFProject.h"

UBTService_AttackManager::UBTService_AttackManager()
	: PunchCooldown(0.5f)
	, HeadbuttCooldown(3.0f)
	, PunchRange(200.f)
	, HeadbuttRange(400.f)
	, LastPunchTime(-999.f)
	, LastHeadbuttTime(-999.f)
{
	NodeName = TEXT("AttackManager");
	Interval = 0.1f;

	TargetKey = TEXT("TargetActor");
	SelectedAttackTypeKey = TEXT("AttackType");
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

	const float Distance = FVector::Dist(MyCharacter->GetActorLocation(), Target->GetActorLocation());
	const float TimeNow = MyCharacter->GetWorld()->GetTimeSeconds();

	TArray<TPair<uint8, float>> Candidates;

	if (Distance <= PunchRange && TimeNow - LastPunchTime >= PunchCooldown)
	{
		Candidates.Add({ 1, PunchRange });
	}
	if (Distance <= HeadbuttRange && TimeNow - LastHeadbuttTime >= HeadbuttCooldown)
	{
		Candidates.Add({ 2, HeadbuttRange });
	}

	if (Candidates.Num() > 0)
	{
		Candidates.Sort([](const TPair<uint8, float>& A, const TPair<uint8, float>& B)
			{
				return A.Value > B.Value;
			});

		uint8 Selected = Candidates[0].Key;

		if (Selected == 1) LastPunchTime = TimeNow;
		else if (Selected == 2) LastHeadbuttTime = TimeNow;

		BlackboardComp->SetValueAsEnum(SelectedAttackTypeKey, Selected);
	}
	else
	{
		BlackboardComp->SetValueAsEnum(SelectedAttackTypeKey, 0); // None
	}
}