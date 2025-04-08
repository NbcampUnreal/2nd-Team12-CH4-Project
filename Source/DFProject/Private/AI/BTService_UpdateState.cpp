// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_UpdateState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Character/DFCharacter.h"
#include "Kismet/GameplayStatics.h" 
#include "DFProject.h"

UBTService_UpdateState::UBTService_UpdateState()
{
	Interval = 0.5f;
	NodeName = TEXT("UpdateState");
	AttackRange = 300.f; 
	DistanceKey = TEXT("DistanceToTarget");
	IsInAttackRangeKey = TEXT("IsInAttackRange");
}

void UBTService_UpdateState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		LOG_ERROR(TEXT("No AIController in UpdateState."));
		return;
	}

	ADFCharacter* MyCharacter = Cast<ADFCharacter>(AIController->GetPawn());
	if (!MyCharacter)
	{
		LOG_ERROR(TEXT("No DFCharacter in UpdateState."));
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		LOG_ERROR(TEXT("No BlackboardComponent in UpdateState."));
		return;
	}

	TArray<AActor*> FoundEnemy;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADFCharacter::StaticClass(), FoundEnemy);
	
	AActor* NearestEnemy = nullptr;
	float BestDistance = FLT_MAX;

	for (AActor* Actor : FoundEnemy)
	{
		if (Actor == MyCharacter)
		{
			continue;
		}

		float CurrentDistance = FVector::Dist(MyCharacter->GetActorLocation(), Actor->GetActorLocation());
		if (CurrentDistance < BestDistance)
		{
			BestDistance = CurrentDistance;
			NearestEnemy = Actor;
		}
	}

	if (NearestEnemy)
	{
		BlackboardComp->SetValueAsObject(TEXT("TargetActor"), NearestEnemy);
		BlackboardComp->SetValueAsFloat(DistanceKey, BestDistance);
		BlackboardComp->SetValueAsBool(IsInAttackRangeKey, BestDistance <= AttackRange);
		LOG(Log, TEXT("UpdateState: Nearest enemy = %s, Distance = %f, InAttackRange = %s"),
			*NearestEnemy->GetName(),
			BestDistance,
			(BestDistance <= AttackRange ? TEXT("true") : TEXT("false")));
	}
	else
	{
		BlackboardComp->ClearValue(TEXT("TargetActor"));
		BlackboardComp->SetValueAsFloat(DistanceKey, 0.f);
		BlackboardComp->SetValueAsBool(IsInAttackRangeKey, false);
		LOG_WARNING(TEXT("No enemy found in UpdateState."));
	}
}
