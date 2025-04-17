#include "AI/DFAIController.h"
#include "DFProject.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h" 
#include "Character/DFCharacter.h"

ADFAIController::ADFAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BrainComponent"));
	CurrentAILevel = EAI_AILevels::Expert;
}

void ADFAIController::BeginPlay()
{
	Super::BeginPlay();

	APawn* ControlledPawn = GetPawn();
	BeginAI(ControlledPawn);
}

void ADFAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndAI();
	Super::EndPlay(EndPlayReason);
}

void ADFAIController::BeginAI(APawn* InPawn)
{
	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(Blackboard);
	if (IsValid(BlackboardComponent))
	{
		if (UseBlackboard(BlackboardDataAsset, BlackboardComponent))
		{
			/*if (ADFCharacter* AIChar = Cast<ADFCharacter>(InPawn))
			{
				if (AIChar->HasAuthority())
				{
					SetAITeamID(1); 
					LOG_WARNING(TEXT("BeginAI: Set TeamID = 1"));
				}
			}*/

			BlackboardComponent->SetValueAsEnum(TEXT("AILevel"), static_cast<int32>(CurrentAILevel));

			const bool bRunSucceeded = RunBehaviorTree(BehaviorTree);
			CHECK(bRunSucceeded != BehaviorTree, );
			LOG(Log, TEXT("Run Behavior Tree"));
			LOG_WARNING(TEXT("BeginAI: CurrentAILevel = %d"), static_cast<int32>(CurrentAILevel));
		}
	}
}

void ADFAIController::EndAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (IsValid(BehaviorTreeComponent) == true)
	{
		BehaviorTreeComponent->StopTree();
	}
}

void ADFAIController::Server_SetAILevel_Implementation(EAI_AILevels NewLevel)
{
	CurrentAILevel = NewLevel;

	if (UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(Blackboard))
	{
		BlackboardComponent->SetValueAsEnum(TEXT("AILevel"), static_cast<int32>(CurrentAILevel));
	}
}

bool ADFAIController::Server_SetAILevel_Validate(EAI_AILevels NewLevel)
{
	return true;
}

EAI_AILevels ADFAIController::GetAILevel() const
{
	return CurrentAILevel;
}

//void ADFAIController::SetAITeamID(int32 InTeamID)
//{
//	if (ADFCharacter* MyCharacter = Cast<ADFCharacter>(GetPawn()))
//	{
//		MyCharacter->Server_SetTeamID(InTeamID);
//	}
//}