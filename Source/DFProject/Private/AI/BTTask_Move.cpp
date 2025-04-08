#include "AI/BTTask_Move.h"
#include "AIController.h"
#include "Character/DFCharacter.h"
#include "DFProject.h"

UBTTask_Move::UBTTask_Move()
{
	NodeName = TEXT("Move");

	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Move::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		LOG_ERROR(TEXT("No AI Controller in MoveTask"));
		return EBTNodeResult::Failed;
	}

	ADFCharacter* MyCharacter = Cast<ADFCharacter>(AIController->GetPawn());
	if (!MyCharacter)
	{
		LOG_ERROR(TEXT("No DFCharacter in MoveTask"));
		return EBTNodeResult::Failed;
	}

	if (ShouldJump())
	{
		MyCharacter->Jump();
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		LOG_ERROR(TEXT("No BlackboardComponent in MoveNode."));
		return EBTNodeResult::Failed;
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTTask_Move::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		LOG_ERROR(TEXT("No AIController in TickTask."));
		return;
	}

	ADFCharacter* MyCharacter = Cast<ADFCharacter>(AIController->GetPawn());
	if (!MyCharacter)
	{
		LOG_ERROR(TEXT("No DFCharacter in TickTask."));
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		LOG_ERROR(TEXT("No BlackboardComponent in TickTask."));
		return;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));
	if (TargetActor)
	{
		FVector Direction = TargetActor->GetActorLocation() - MyCharacter->GetActorLocation();
		Direction.Z = 0.f;
		if (!Direction.IsNearlyZero())
		{
			Direction.Normalize();

			FRotator CurrentRotation = MyCharacter->GetActorRotation();
			FRotator DesiredRotation = Direction.Rotation();

			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaSeconds, 5.f);

			MyCharacter->SetActorRotation(NewRotation);

			LOG(Log, TEXT("TickTask: Rotating from %s to %s"), *CurrentRotation.ToString(), *NewRotation.ToString());
		}
		else
		{
			LOG_WARNING(TEXT("No TargetActor in Blackboard."));
		}
	}
}

bool UBTTask_Move::ShouldJump() const
{
	return false;
}
