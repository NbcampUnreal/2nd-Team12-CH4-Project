// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/State/DeadState.h"

#include "Character/DFCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

void UDeadState::Enter(ADFCharacter* Character)
{
	Super::Enter(Character);
	// 잡혀있다면 끊기
	if (Character->GrabberConstraint.Get() && !Character->GrabberConstraint->IsBroken())
	{
		Character->GrabberConstraint->BreakConstraint();
	}

	// 잡은 것들도 다 놓기
	Character->Server_ReleaseGrab();

	// 파티클이랑 사운드 추가?

	Character->SetActorHiddenInGame(true);
	Character->SetActorEnableCollision(false);
	if (UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(Character->GetMovementComponent()))
	{
		MoveComp->StopMovementImmediately();
	}
}

void UDeadState::Exit(ADFCharacter* Character)
{
	Super::Exit(Character);
}

void UDeadState::Tick(ADFCharacter* Character, float DeltaTime)
{
	Super::Tick(Character, DeltaTime);
}

bool UDeadState::CanChangeToState(ECharacterStateType NewState)
{
	return NewState == ECharacterStateType::Idle;
}
