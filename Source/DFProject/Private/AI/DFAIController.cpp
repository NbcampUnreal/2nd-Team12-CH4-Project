#include "AI/DFAIController.h"
#include "DFProject.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h" // 추가: 액터 검색용
#include "Character/DFCharacter.h"

ADFAIController::ADFAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BrainComponent"));
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
			bool bRunSucceeded = RunBehaviorTree(BehaviorTree);

			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADFCharacter::StaticClass(), FoundActors);
			AActor* ChosenTarget = nullptr;
			for (AActor* Actor : FoundActors)
			{
				if (Actor != GetPawn())
				{
					ChosenTarget = Actor;
					break;
				}
			}

			if (ChosenTarget)
			{
				BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), ChosenTarget);
				LOG(Log, TEXT("TargetActor : %s"), *ChosenTarget->GetName());
			}
			else
			{
				LOG_WARNING(TEXT("No valid TargetActor."));
			}
			
			CHECK(bRunSucceeded != BehaviorTree, )
			LOG(Log, TEXT("Run Behavior Tree"))
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