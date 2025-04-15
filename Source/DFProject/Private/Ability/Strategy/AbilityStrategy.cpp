// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Strategy/AbilityStrategy.h"

void UAbilityStrategy::StartAbility_Implementation(AActor* TargetActor)
{
	if (!CanActivateAbility(TargetActor)) return;
	
	bIsAbilityActive = true;
	LastUsedTime = GetWorld()->GetTimeSeconds();
	ActivateAbility(TargetActor);
}

void UAbilityStrategy::EndAbility_Implementation(AActor* TargetActor)
{
	bIsAbilityActive = false;
}

bool UAbilityStrategy::CanActivateAbility_Implementation(AActor* TargetActor)
{
	if (!TargetActor->HasAuthority()) return false; 
	if (bIsAbilityActive) return false; //이미 실행 중

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if ((CurrentTime - LastUsedTime) < CoolTime) return false; // 쿨타임

	if (!TargetActor) return false;
	
	return true;
}

void UAbilityStrategy::ActivateAbility_Implementation(AActor* TargetActor)
{
}
