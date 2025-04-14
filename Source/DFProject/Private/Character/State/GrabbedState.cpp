// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/State/GrabbedState.h"

void UGrabbedState::Enter(ADFCharacter* Character)
{
	Super::Enter(Character);
}

void UGrabbedState::Tick(ADFCharacter* Character, float DeltaTime)
{
	Super::Tick(Character, DeltaTime);
}

void UGrabbedState::Exit(ADFCharacter* Character)
{
	Super::Exit(Character);
}
