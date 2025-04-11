// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Grab/GrabMovementModifier.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGrabMovementModifier::UGrabMovementModifier()
{
	PrimaryComponentTick.bCanEverTick = false;
	PullStrength = 1.f;
	CurrentInterpolatedStrength = 0.f;
}


void UGrabMovementModifier::BeginPlay()
{
	Super::BeginPlay();
}


void UGrabMovementModifier::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!TargetActor.IsValid())
		return;

	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;

	UCharacterMovementComponent* MoveComp = OwnerChar->GetCharacterMovement();
	if (!MoveComp) return;

	FVector TargetLocation = TargetActor->GetActorLocation();
	FVector MyLocation = OwnerChar->GetActorLocation();

	FVector InputDirection = MoveComp->GetLastInputVector().GetSafeNormal();
	FVector MyVelocity = MoveComp->Velocity;
	float MyStrength = CalculateStrength(MoveComp);

	ACharacter* TargetChar = Cast<ACharacter>(TargetActor.Get());
	UCharacterMovementComponent* TargetMoveComp = TargetChar ? TargetChar->GetCharacterMovement() : nullptr;

	//FVector TargetVelocity = TargetMoveComp ? TargetMoveComp->Velocity : FVector::ZeroVector;
	//float TargetStrength = TargetMoveComp ? CalculateStrength(TargetMoveComp) : EstimatedStaticStrength;
//
	//FVector PullDirection = (TargetLocation - MyLocation).GetSafeNormal();
	//float PullForce = TargetStrength - MyStrength;
//
	//if (PullForce > 0.f)
	//{
	//	FVector Adjusted = PullDirection * PullForce;
	//	MoveComp->Velocity = FMath::VInterpTo(MoveComp->Velocity, Adjusted, DeltaTime, 5.f);
	//}
	//else
	//{
	//	
	//}
	//

	// 힘 비교 후 힘 차이를 구하고 약한 쪽을 강한 쪽으로 SetLocation 해서 끌려가는 것 처럼 속이기
	// 상대가 움직이지 않는 액터라고 해도 무게가 무겁다면 캐릭터의 속력이 느려져야함.
	// 밀어내기는 아직 계획이 없음. 밀어내는 동안엔 움직이지 않는 액터처럼 속력이 느려지는 방식을
	
	// 디버그용
	if (bEnableDebugDraw)
	{
		//FVector Start = OwnerChar->GetActorLocation();
		//FVector End = Start + FinalInput * 300.f;
		//DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 0.05f, 0, 2.f);
	}
}

void UGrabMovementModifier::ActivateModifier(AActor* InTargetActor)
{
	// 타겟에도 캐릭터 무브먼트가 있을때만 사용하자.
	TargetActor = InTargetActor;
	SetComponentTickEnabled(true);
}

void UGrabMovementModifier::DeactivateModifier()
{
	TargetActor = nullptr;
	SetComponentTickEnabled(false);
}

float UGrabMovementModifier::CalculateStrength(UCharacterMovementComponent* MoveComp) const
{
	// 질량은 메시에서? 잡고 있는 액터의 무게만큼 속력이 감소해야함. movement comp가 없어도 힘 계산 필요
	MoveComp->Velocity;
	return 0.f;
}

