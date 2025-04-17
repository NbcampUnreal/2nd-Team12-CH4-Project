// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_GrabManager.h"
#include "AI/DFAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/DFCharacter.h"
#include "Character/State/CharacterStateManager.h"
#include "Character/State/CharacterStateBase.h" 
#include "Kismet/GameplayStatics.h"
#include "DFProject.h"

UBTService_GrabManager::UBTService_GrabManager()
{
	NodeName = TEXT("GrabManager");

	Interval = 0.3f;
	RandomDeviation = 0.f;

	GrabTargetActorKey = TEXT("GrabTargetActor");
	CanGrabKey = TEXT("bCanGrab");
	AILevelKey = TEXT("AILevel");
	GrabDetectRadius = 800.f;
}

void UBTService_GrabManager::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ADFAIController* AIController = Cast<ADFAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return;
	}

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	uint8 AILevelRaw = BlackboardComp->GetValueAsEnum(AILevelKey);
	EAI_AILevels AILevel = static_cast<EAI_AILevels>(AILevelRaw);
	if (AILevel != EAI_AILevels::Expert)
	{
		BlackboardComp->ClearValue(GrabTargetActorKey);
		BlackboardComp->SetValueAsBool(CanGrabKey, false);
		LOG_WARNING(TEXT("GrabManager: AILevel < Expert → Grab 탐색 스킵"));
		return;
	}

	AActor* ClosestStunned = nullptr;
	float ClosestDistance = FLT_MAX;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(AIPawn->GetWorld(), ADFCharacter::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		ADFCharacter* Enemy = Cast<ADFCharacter>(Actor);
		if (!Enemy || Enemy == AIPawn || !Enemy->StateManager || !Enemy->StateManager->CurrentState)
			continue;

		if (Enemy->StateManager->CurrentState->GetStateType() != ECharacterStateType::Stunned)
			continue;

		float Distance = FVector::Dist2D(AIPawn->GetActorLocation(), Enemy->GetActorLocation());
		if (Distance <= GrabDetectRadius && Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestStunned = Enemy;
		}
	}

	if (ClosestStunned)
	{
		BlackboardComp->SetValueAsObject(GrabTargetActorKey, ClosestStunned);
		BlackboardComp->SetValueAsBool(CanGrabKey, true);

	}
	else
	{
		BlackboardComp->ClearValue(GrabTargetActorKey);
		BlackboardComp->SetValueAsBool(CanGrabKey, false);
	}
}
