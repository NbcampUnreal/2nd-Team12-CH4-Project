// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Strategy/AbilityStrategyManager.h"

#include "Ability/Strategy/AbilityStrategy.h"

void UAbilityStrategyManager::RegisterAbility(FName AbilityName, UAbilityStrategy* Ability)
{
	if (!Ability || RegisteredAbilities.Contains(AbilityName)) return;
	RegisteredAbilities.Add(AbilityName, Ability);
}

void UAbilityStrategyManager::StartAbility(FName AbilityName, AActor* TargetActor)
{
	if (UAbilityStrategy* Ability = RegisteredAbilities.FindRef(AbilityName))
	{
		Ability->StartAbility(TargetActor);
	}
}

void UAbilityStrategyManager::StopAbility(FName AbilityName, AActor* TargetActor)
{
	if (UAbilityStrategy* Ability = RegisteredAbilities.FindRef(AbilityName))
	{
		Ability->EndAbility(TargetActor);
	}
}

bool UAbilityStrategyManager::IsAbilityActive(FName AbilityName) const
{
	if (const UAbilityStrategy* Ability = RegisteredAbilities.FindRef(AbilityName))
	{
		return Ability->bIsAbilityActive;
	}
	return false;
}

void UAbilityStrategyManager::ClearAllAbilities()
{
	RegisteredAbilities.Empty();
}

void UAbilityStrategyManager::InitializeAbilities()
{
	for (auto& Pair : InitialAbilities)
	{
		if (Pair.Value)
		{
			if (UAbilityStrategy* NewAbility = NewObject<UAbilityStrategy>(this, Pair.Value))
			{
				RegisteredAbilities.Add(Pair.Key, NewAbility);
			}
		}
	}
}
