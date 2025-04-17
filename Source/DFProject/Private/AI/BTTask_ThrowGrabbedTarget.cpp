// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ThrowGrabbedTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/DFAIController.h"
#include "Character/DFCharacter.h"
#include "Character/State/CharacterStateManager.h"
#include "Character/State/CharacterStateBase.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "DFProject.h"

UBTTask_ThrowGrabbedTarget::UBTTask_ThrowGrabbedTarget()
{
	NodeName = TEXT("ThrowGrabbedTarget");

	GrabTargetActorKey = TEXT("GrabTargetActor");
	ThrowPower = 1000.f;
	ThrowDelay = 0.3f; 
	bNotifyTick = false;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_ThrowGrabbedTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADFAIController* AIController = Cast<ADFAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		LOG_ERROR(TEXT("DFAIController not found"));
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
		LOG_WARNING(TEXT("ExecuteTask : BlackboardComponent not found"));
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(GrabTargetActorKey));
	ADFCharacter* TargetCharacter = Cast<ADFCharacter>(TargetActor);

	if (!TargetCharacter)
	{
		LOG_WARNING(TEXT("ExecuteTask : No Grabbed Target Character"));
		return EBTNodeResult::Failed;
	}

	return EvaluateAndAttemptThrow(OwnerComp, MyCharacter, TargetCharacter);
}

EBTNodeResult::Type UBTTask_ThrowGrabbedTarget::EvaluateAndAttemptThrow(UBehaviorTreeComponent& OwnerComp, ADFCharacter* MyCharacter, ADFCharacter* TargetCharacter)
{
	if (!MyCharacter)
	{
		LOG_WARNING(TEXT("ThrowTask: MyCharacter X"));
		return EBTNodeResult::Failed;
	}

	if (!TargetCharacter)
	{
		LOG_WARNING(TEXT("ThrowTask: GrabTargetCharacter X"));
		return EBTNodeResult::Failed;
	}

	if (!TargetCharacter->StateManager || !TargetCharacter->StateManager->CurrentState)
	{
		LOG_WARNING(TEXT("ThrowTask: State X"));
		return EBTNodeResult::Failed;
	}

	const ECharacterStateType TargetState = TargetCharacter->StateManager->CurrentState->GetStateType();
	if (TargetState != ECharacterStateType::Stunned)
	{
		LOG_WARNING(TEXT("ThrowTask: Target Isnt Stunned "));
		MyCharacter->Server_ReleaseGrab();
		MyCharacter->Server_StopGrab();
		return EBTNodeResult::Failed;
	}

	MyCharacter->Jump();
	LOG_WARNING(TEXT("ThrowTask: Jump"));

	OwnerCompPtr = &OwnerComp;

	FTimerDelegate ThrowDelegate;
	ThrowDelegate.BindLambda([=, this]()
		{
			if (!MyCharacter || !TargetCharacter) return;

			MyCharacter->Server_ReleaseGrab();
			MyCharacter->Server_StopGrab();

			if (UPrimitiveComponent* Mesh = TargetCharacter->GetMesh())
			{
				const FVector ThrowDir = (TargetCharacter->GetActorLocation() - MyCharacter->GetActorLocation()).GetSafeNormal();
				Mesh->AddImpulse(ThrowDir * ThrowPower, NAME_None, true);
				LOG_WARNING(TEXT("ThrowTask: Impulse O"));
			}
			else
			{
				LOG_WARNING(TEXT("ThrowTask: TargetCharacter Mesh X"));
			}

			if (OwnerCompPtr)
			{
				FinishLatentTask(*OwnerCompPtr, EBTNodeResult::Succeeded);
			}
		});

	MyCharacter->GetWorld()->GetTimerManager().SetTimer(
		ThrowTimerHandle, ThrowDelegate, ThrowDelay, false);

	return EBTNodeResult::InProgress;
}