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
	
	const uint8 RawAILevel = BlackboardComp->GetValueAsEnum(AILevelKey);
	EAI_AILevels AILevel = static_cast<EAI_AILevels>(RawAILevel);

	float HeadbuttRate = 0.f;
	switch (AILevel)
	{
	case EAI_AILevels::Basic: HeadbuttRate = 0.3f; break;
	case EAI_AILevels::Expert: HeadbuttRate = 0.4f; break;
	default: HeadbuttRate = 0.f; break; // Rookie 등
	}

	const float RandValue = FMath::FRand();

	if (RandValue < HeadbuttRate)
	{
		HandleHeadbutt(MyCharacter);
		LOG_WARNING(TEXT("AttackTask: 선택된 공격 = Headbutt (%.2f)"), RandValue);
	}
	else
	{
		HandlePunch(MyCharacter);
		LOG_WARNING(TEXT("AttackTask: 선택된 공격 = Punch (%.2f)"), RandValue);
	}

	return EBTNodeResult::Succeeded;
}

void UBTTask_Attack::HandlePunch(ADFCharacter* MyCharacter)
{
	if (!MyCharacter) return;

	MyCharacter->GetWorldTimerManager().ClearTimer(PunchTimerHandle);

	MyCharacter->Server_Punch();
	LOG_WARNING(TEXT("AttackTask: Punch (첫번째 팔)"));

	MyCharacter->GetWorldTimerManager().SetTimer(
		PunchTimerHandle,
		[MyCharacter]() {
			if (MyCharacter && MyCharacter->StateManager->IsCurrentState(ECharacterStateType::Idle))
			{
				MyCharacter->Server_Punch();
				LOG_WARNING(TEXT("AttackTask: Punch (두번째 팔)"));
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