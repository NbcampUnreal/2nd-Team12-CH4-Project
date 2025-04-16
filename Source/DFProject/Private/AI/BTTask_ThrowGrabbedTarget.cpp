// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ThrowGrabbedTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Character/DFCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DFProject.h"

UBTTask_ThrowGrabbedTarget::UBTTask_ThrowGrabbedTarget()
{
	NodeName = TEXT("ThrowGrabbedTarget");

	GrabTargetActorKey = TEXT("GrabTargetActor");
	DangerDirectionKey = TEXT("DangerDirection");

	ThrowPower = 1200.f;
	ThrowDelay = 0.3f;

	bNotifyTick = false;
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

	FTimerDelegate ThrowDelegate;
	ThrowDelegate.BindUObject(this, &UBTTask_ThrowGrabbedTarget::DelayedThrow, &OwnerComp);

	MyCharacter->GetWorldTimerManager().SetTimer(
		DelayHandle,
		ThrowDelegate,
		ThrowDelay,
		false
	);
	return EBTNodeResult::InProgress;
}

void UBTTask_ThrowGrabbedTarget::DelayedThrow(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp) return;

	AAIController* AIController = OwnerComp->GetAIOwner();
	if (!AIController) return;

	ADFCharacter* MyCharacter = Cast<ADFCharacter>(AIController->GetPawn());
	if (!MyCharacter) return;

	UBlackboardComponent* BlackboardComp = OwnerComp->GetBlackboardComponent();
	if (!BlackboardComp) return;

	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(GrabTargetActorKey));
	if (!Target)
	{
		LOG_WARNING(TEXT("Throw: GrabTargetActor 없음"));
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const FVector DangerDir = BlackboardComp->GetValueAsVector(DangerDirectionKey);
	if (DangerDir.IsNearlyZero())
	{
		LOG_WARNING(TEXT("Throw: DangerDirection 없음"));
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 던지기 전 그랩 해제
	MyCharacter->Server_ReleaseGrab();

	// Impulse 적용 (Z도 살짝 넣어 위로 던짐)
	const FVector LaunchImpulse = DangerDir.GetSafeNormal() * ThrowPower + FVector(0.f, 0.f, 300.f);

	UPrimitiveComponent* TargetRoot = Cast<UPrimitiveComponent>(Target->GetRootComponent());
	if (TargetRoot && TargetRoot->IsSimulatingPhysics())
	{
		TargetRoot->AddImpulse(LaunchImpulse, NAME_None, true);
		LOG_WARNING(TEXT("🎯 던지기 Impulse 적용 → %s | 방향: %s"), *Target->GetName(), *LaunchImpulse.ToString());
	}
	else
	{
		LOG_WARNING(TEXT("Throw: 대상이 물리 적용 안 됨 → %s"), *Target->GetName());
	}

	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}