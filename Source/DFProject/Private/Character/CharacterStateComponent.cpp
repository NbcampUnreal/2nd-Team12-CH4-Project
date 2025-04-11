// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterStateComponent.h"


void UCharacterStateComponent::AddState(ECharacterState NewState)
{
	CurrentState |= NewState;
}

void UCharacterStateComponent::RemoveState(ECharacterState StateToRemove)
{
	CurrentState &= ~StateToRemove;
}

bool UCharacterStateComponent::IsInState(ECharacterState StateToCheck) const
{
	return EnumHasAllFlags(CurrentState, StateToCheck);
}
