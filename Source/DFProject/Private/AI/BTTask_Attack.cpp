#include "AI/BTTask_Attack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Character/DFCharacter.h"
#include "DFProject.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");
	SelectedAttackTypeKey = TEXT("AttackType");
	bNotifyTick = true;

	PunchInterval = 0.2f;
	ElapsedTime = 0.f;
	CurrentPunchCount = 0;
	bIsPunching = false;
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
		// Safe Code
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

	const uint8 RawType = BlackboardComp->GetValueAsEnum(SelectedAttackTypeKey);

	if (RawType == 2) // Headbutt
	{
		MyCharacter->Server_Headbutt();
		BlackboardComp->SetValueAsEnum(SelectedAttackTypeKey, 0);
		return EBTNodeResult::Succeeded;
	}
	else if (RawType == 1) // Punch
	{
		// 초기화 및 첫 펀치
		MyCharacter->Server_Punch();
		bIsPunching = true;
		ElapsedTime = 0.f;
		CurrentPunchCount = 1;
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!bIsPunching) return;

	ElapsedTime += DeltaSeconds;
	if (ElapsedTime >= PunchInterval)
	{
		ElapsedTime = 0.f;
		++CurrentPunchCount;

		AAIController* AIController = OwnerComp.GetAIOwner();
		ADFCharacter* MyCharacter = Cast<ADFCharacter>(AIController->GetPawn());

		if (MyCharacter)
		{
			MyCharacter->Server_Punch();
		}

		if (CurrentPunchCount >= 6)
		{
			UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
			if (BlackboardComp)
			{
				BlackboardComp->SetValueAsEnum(SelectedAttackTypeKey, 0);
			}

			bIsPunching = false;
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}
	/*const float Now = MyCharacter->GetWorld()->GetTimeSeconds();

	if (BlackboardComp->GetValueAsBool(TEXT("IsInHeadbuttRange")) && !BlackboardComp->GetValueAsBool(TEXT("IsHeadbuttCooldown")))
	{
		MyCharacter->Server_Headbutt_Implementation();
		BlackboardComp->SetValueAsFloat(TEXT("LastHeadbuttTime"), Now);
		LOG(Log, TEXT("Start Headbutt Attack!!!!!"));
		return EBTNodeResult::Succeeded;
	}

	if (BlackboardComp->GetValueAsBool(TEXT("IsInPunchRange")) && !BlackboardComp->GetValueAsBool(TEXT("IsPunchCooldown")))
	{
		MyCharacter->Server_Punch();
		BlackboardComp->SetValueAsFloat(TEXT("LastPunchTime"), Now);
		LOG(Log, TEXT("Start Punch Attack!!!!!"));
		return EBTNodeResult::Succeeded;
	}

	LOG_WARNING(TEXT("AttackTask: No attack condition met."));
	return EBTNodeResult::Failed;*/

	//const bool bCanAttack = BlackboardComp->GetValueAsBool(TEXT("IsInAttackRange"));

	//if (bCanAttack)
	//{
	//	MyCharacter->Server_Punch();
	//	LOG(Log, TEXT("AttackTask: In range, punch executed."));
	//	return EBTNodeResult::Succeeded;
	//}
	//else
	//{
	//	LOG_WARNING(TEXT("AttackTask: Not in attack range."));
	//	return EBTNodeResult::Failed;
	//}
	
//}

