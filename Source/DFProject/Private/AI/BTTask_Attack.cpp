#include "AI/BTTask_Attack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Character/DFCharacter.h"
#include "DFProject.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		LOG_ERROR(TEXT("AIController is not in Attack Task."));
		return EBTNodeResult::Failed;
	}

	ADFCharacter* MyCharacter = Cast<ADFCharacter>(AIController->GetPawn());
	if (!MyCharacter)
	{
		LOG_ERROR(TEXT("Pawn is not a DFCharacter."));
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		LOG_ERROR(TEXT("Can't find Blackboard Component."));
		return EBTNodeResult::Failed;
	}

	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));
	if (!Target)
	{
		LOG_WARNING(TEXT("Cant found TargetActor in Blackboard."));
		return EBTNodeResult::Failed;
	}

	float Distance = BlackboardComp->GetValueAsFloat(TEXT("DistanceToTarget"));
	LOG(Log, TEXT("Attack Task : DistanceToTarget = %f"), Distance);

	return EBTNodeResult::Succeeded;
}

