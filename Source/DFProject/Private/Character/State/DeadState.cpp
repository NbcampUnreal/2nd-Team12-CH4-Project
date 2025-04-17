// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/State/DeadState.h"

void UDeadState::Enter(ADFCharacter* Character)
{
	Super::Enter(Character);
	// 잡기 끊기
	//
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
