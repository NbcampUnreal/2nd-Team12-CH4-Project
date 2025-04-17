#include "AI/BTTask_Attack.h"
#include "AI/DFAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/DFCharacter.h"
#include "Character/State/CharacterStateManager.h"
#include "Character/State/CharacterStateBase.h"
#include "DFProject.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");
	AILevelKey = TEXT("AILevel");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	const EAI_AILevels AILevel = AIController->GetAILevel();

	float HeadbuttRate = 0.f;
	switch (AILevel)
	{
	case EAI_AILevels::Basic: HeadbuttRate = 0.3f; break;
	case EAI_AILevels::Expert: HeadbuttRate = 0.4f; break;
	default: HeadbuttRate = 0.f; break; 
	}

	const float RandValue = FMath::FRand();

	if (RandValue < HeadbuttRate)
	{
		HandleHeadbutt(MyCharacter);
	}
	else
	{
		HandlePunch(MyCharacter);
	}

	return EBTNodeResult::Succeeded;
}

void UBTTask_Attack::HandlePunch(ADFCharacter* MyCharacter)
{
	if (!MyCharacter) return;

	MyCharacter->GetWorldTimerManager().ClearTimer(PunchTimerHandle);

	MyCharacter->Server_Punch();

	MyCharacter->GetWorldTimerManager().SetTimer(
		PunchTimerHandle,
		[MyCharacter]() {
			if (MyCharacter && MyCharacter->StateManager->IsCurrentState(ECharacterStateType::Idle))
			{
				MyCharacter->Server_Punch();
			}
		},
		0.5f, false);
}

void UBTTask_Attack::HandleHeadbutt(ADFCharacter* MyCharacter)
{
	if (!MyCharacter) return;
	MyCharacter->Server_Headbutt();
	LOG_WARNING(TEXT("AttackTask: Headbutt"));
}