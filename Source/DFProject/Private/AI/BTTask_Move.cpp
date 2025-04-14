#include "AI/BTTask_Move.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/DFCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Level/DFDeadZoneComponent.h"
#include "DFProject.h"

UBTTask_Move::UBTTask_Move()
{
	NodeName = TEXT("Move");

	bNotifyTick = true;

	bWasFalling = false;
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

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		LOG_ERROR(TEXT("No BlackboardComponent in MoveTask."));
		return EBTNodeResult::Failed;
	}

	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));
	if (!Target)
	{
		LOG_WARNING(TEXT("No TargetActor in Blackboard."));
		return EBTNodeResult::Failed;
	}

	if (ShouldJump(MyCharacter))
	{
		LOG(Log, TEXT("Find Obstacle. Start Jump."));

		//FVector MoveDirection = (Target->GetActorLocation() - MyCharacter->GetActorLocation()).GetSafeNormal2D();

		//MyCharacter->AddMovementInput(MoveDirection, 1.0f);

		MyCharacter->Jump();

		MyCharacter->GetCharacterMovement()->MaxWalkSpeed = 800.f;

		BlackboardComp->SetValueAsBool(TEXT("ShouldJump"), true);
		BlackboardComp->SetValueAsBool(TEXT("IsJumping"), true);
	}
	else
	{
		BlackboardComp->SetValueAsBool(TEXT("ShouldJump"), false);
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
	if (!TargetActor)
	{
		LOG_ERROR(TEXT("No TargetActor in TickTask."));
		return;
	}

	if (MyCharacter->GetCharacterMovement()->IsFalling())
	{
		FVector MoveDirection = (TargetActor->GetActorLocation() - MyCharacter->GetActorLocation()).GetSafeNormal2D();
		MyCharacter->AddMovementInput(MoveDirection, 1.0f);

		bWasFalling = true;

		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsBool(TEXT("IsJumping"), true);
		}
	}
	else
	{
		if (bWasFalling)
		{
			MyCharacter->GetCharacterMovement()->MaxWalkSpeed = 450.f;
			bWasFalling = false;

			if (BlackboardComp)
			{
				BlackboardComp->SetValueAsBool(TEXT("IsJumping"), false);
			}

		}
	}
	
	FVector Direction = TargetActor->GetActorLocation() - MyCharacter->GetActorLocation();
	Direction.Z = 0.f;

	if (!Direction.IsNearlyZero())
	{
		Direction.Normalize();

		FRotator CurrentRotation = MyCharacter->GetActorRotation();
		FRotator DesiredRotation = Direction.Rotation();

		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaSeconds, 5.f);

		MyCharacter->SetActorRotation(NewRotation);

		//LOG(Log, TEXT("TickTask: Rotating from %s to %s"), *CurrentRotation.ToString(), *NewRotation.ToString());
	}
	else
	{
		LOG_WARNING(TEXT("No TargetActor in Blackboard."));
	}
		
}

bool UBTTask_Move::ShouldJump(ADFCharacter* MyCharacter) const
{
	UWorld* World = MyCharacter->GetWorld();
	if (!World)
	{
		LOG_ERROR(TEXT("No World in ShouldJump."));
		return false;
	}

	FVector Start = MyCharacter->GetActorLocation() + FVector(0.f, 0.f, 50.f);
	FVector End = Start + MyCharacter->GetActorForwardVector() * 200.f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MyCharacter);

	bool bHit = World->SweepSingleByChannel
	(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeCapsule(30.f, 50.f),
		Params
	);

	bool bHasValidObstacle = false;

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();

		if (HitActor && HitActor->GetClass()->ImplementsInterface(UGrabbable::StaticClass()))
		{
			LOG_WARNING(TEXT("ShouldJump: HitActor is Grabbable. Ignore jump."));
			return false;
		}

		if (HitActor && HitActor->FindComponentByClass<UDFDeadZoneComponent>())
		{
			LOG_WARNING(TEXT("ShouldJump: HitActor has DFDeadZoneComponent. Ignore jump."));
			return false;
		}


		if (HitActor && HitActor->IsA<ADFCharacter>())
		{
			FHitResult ObstacleHit;

			FVector ObstacleStart = MyCharacter->GetActorLocation() + FVector(0.f, 0.f, 50.f);
			FVector ObstacleEnd = ObstacleStart + MyCharacter->GetActorForwardVector() * 200;

			FCollisionQueryParams ObstacleParams;
			ObstacleParams.AddIgnoredActor(MyCharacter);
			ObstacleParams.AddIgnoredActor(HitActor);

			bool bObstacleHit = World->SweepSingleByChannel(
				ObstacleHit,
				ObstacleStart,
				ObstacleEnd,
				FQuat::Identity,
				ECC_Visibility,
				FCollisionShape::MakeBox(FVector(30, 30, 60)),
				ObstacleParams
			);

			if (bObstacleHit)
			{
				float Height = ObstacleHit.ImpactPoint.Z - MyCharacter->GetActorLocation().Z;

				if (Height > 10 && Height < 60)
				{
					bHasValidObstacle = true;
				}
			}
		}
		else
		{
			float Height = HitResult.ImpactPoint.Z - MyCharacter->GetActorLocation().Z;

			if (Height > 10.f && Height < 60.f)
			{
				bHasValidObstacle = true;
			}
		}
	}
	return bHasValidObstacle;
}
