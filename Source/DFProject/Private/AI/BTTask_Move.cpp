#include "AI/BTTask_Move.h"
#include "AIController.h"
#include "Character/DFCharacter.h"
#include "DFProject.h"

UBTTask_Move::UBTTask_Move()
{
	NodeName = TEXT("Move");
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

	return Super::ExecuteTask(OwnerComp, NodeMemory);

}

bool UBTTask_Move::ShouldJump() const
{
	return false;
}
