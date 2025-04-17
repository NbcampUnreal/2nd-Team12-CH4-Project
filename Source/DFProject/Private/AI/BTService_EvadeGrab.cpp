// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_EvadeGrab.h"
#include "AI/DFAIController.h"
#include "Character/DFCharacter.h"
#include "Ability/Grab/GrabComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "DFProject.h"

UBTService_EvadeGrab::UBTService_EvadeGrab()
{
	NodeName = TEXT("Evade Grab Threat");
	TargetActorKey = TEXT("TargetActor");
	EvadeLocationKey = TEXT("EvadeLocation");
	IsEvadingKey = TEXT("IsEvadingGrab");
	Interval = 0.2f;

	EvadeDistance = 600.f;
	DetectRange = 400.f;
}

void UBTService_EvadeGrab::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ADFAIController* AIController = Cast<ADFAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return;
	}

	APawn* AIPawn = AIController->GetPawn();
	ADFCharacter* SelfChar = Cast<ADFCharacter>(AIPawn);
	if (!SelfChar)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey));
	if (!TargetActor) 
	{
		return;
	}

	ADFCharacter* TargetChar = Cast<ADFCharacter>(TargetActor);
	if (!TargetChar) 
	{
		return;
	}

	UGrabComponent* GrabComp = TargetChar->FindComponentByClass<UGrabComponent>();
	if (!GrabComp)
	{
		BlackboardComp->SetValueAsBool(IsEvadingKey, false);
		return;
	}

	bool bIsGrabAttempting = (GrabComp->GetCurrentGrabState() == EGrabState::Detecting);
	float Distance = FVector::Dist(SelfChar->GetActorLocation(), TargetChar->GetActorLocation());
	bool bIsInRange = (Distance <= DetectRange);

	if (bIsGrabAttempting && bIsInRange)
	{
		FVector AwayDir = (SelfChar->GetActorLocation() - TargetChar->GetActorLocation()).GetSafeNormal();
		FVector EvadeLoc = SelfChar->GetActorLocation() + AwayDir * EvadeDistance;

		BlackboardComp->SetValueAsVector(EvadeLocationKey, EvadeLoc);
		BlackboardComp->SetValueAsBool(IsEvadingKey, true);

		LOG_WARNING(TEXT("EvadeGrab: Grab detected → Evading! Distance = %.1f"), Distance);
	}
	else
	{
		BlackboardComp->SetValueAsBool(IsEvadingKey, false);
	}
}
