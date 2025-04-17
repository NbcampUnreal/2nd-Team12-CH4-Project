#include "AI/BTTask_Move.h"
#include "AI/DFAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/DFCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DFProject.h"

UBTTask_Move::UBTTask_Move()
{
	NodeName = TEXT("Move");
	bNotifyTick = true;
	MoveLocationKey = TEXT("MoveLocation"); 
	AttackRange = 500.f;
	DefaultSpeed = 600.f;
}

EBTNodeResult::Type UBTTask_Move::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADFAIController* AIController = Cast<ADFAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		LOG_ERROR(TEXT("No DFAIController in MoveTask"));
		return EBTNodeResult::Failed;
	}

	ADFCharacter* MyCharacter = Cast<ADFCharacter>(AIController->GetPawn());
	if (!MyCharacter)
	{
		LOG_ERROR(TEXT("No DFCharacter in MoveTask"));
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		LOG_ERROR(TEXT("No BlackboardComponent in MoveTask."));
		return EBTNodeResult::Failed;
	}

	FName TargetKeyName = BlackboardComp->GetValueAsName(MoveLocationKey);
	bool bHasLocation = BlackboardComp->IsVectorValueSet(TargetKeyName) || BlackboardComp->GetValueAsObject(TargetKeyName) != nullptr;
	if (!bHasLocation)
	{
		LOG_WARNING(TEXT("MoveTask: TargetKey (%s) has no valid value."), *TargetKeyName.ToString());
		return EBTNodeResult::Failed;
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTTask_Move::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ADFAIController* AIController = Cast<ADFAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return;
	}

	ADFCharacter* MyCharacter = Cast<ADFCharacter>(AIController->GetPawn());
	if (!MyCharacter)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	FName TargetKeyName = BlackboardComp->GetValueAsName(MoveLocationKey);
	FVector TargetLocation = FVector::ZeroVector;

	if (BlackboardComp->IsVectorValueSet(TargetKeyName))
	{
		TargetLocation = BlackboardComp->GetValueAsVector(TargetKeyName);
	}
	else if (AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKeyName)))
	{
		TargetLocation = TargetActor->GetActorLocation();
	}
	else
	{
		LOG_WARNING(TEXT("MoveTask: No valid value at TargetKey = %s"), *TargetKeyName.ToString());
		return;
	}

	float Distance = FVector::Dist(MyCharacter->GetActorLocation(), TargetLocation);
	AdjustSpeed(MyCharacter, AIController, Distance);

	FVector Direction = TargetLocation - MyCharacter->GetActorLocation();
	Direction.Z = 0.f;

	if (!Direction.IsNearlyZero())
	{
		Direction.Normalize();
		FRotator CurrentRotation = MyCharacter->GetActorRotation();
		FRotator DesiredRotation = Direction.Rotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaSeconds, 5.f);
		MyCharacter->SetActorRotation(NewRotation);
	}
}

void UBTTask_Move::AdjustSpeed(ADFCharacter* MyCharacter, ADFAIController* AIController, float Distance)
{
	UCharacterMovementComponent* Movement = MyCharacter->GetCharacterMovement();
	if (!Movement)
	{
		return;
	}

	const float PrevSpeed = Movement->MaxWalkSpeed;	

	float NewSpeed = DefaultSpeed;

	if (Distance < AttackRange && Distance > 300.f)
	{
		EAI_AILevels Level = AIController->GetAILevel();
		switch (Level)
		{
		case EAI_AILevels::Rookie:
			NewSpeed += 0.f;
			break;
		case EAI_AILevels::Basic:
			NewSpeed += 100.f;
			break;
		case EAI_AILevels::Expert:
			NewSpeed += 200.f;
			break;
		default:
			break;
		}
	}

	Movement->MaxWalkSpeed = NewSpeed;

	if (!FMath::IsNearlyEqual(PrevSpeed, Movement->MaxWalkSpeed))
	{
		LOG_WARNING(TEXT("MoveTask: Speed adjusted to %.1f"), Movement->MaxWalkSpeed);
	}
}