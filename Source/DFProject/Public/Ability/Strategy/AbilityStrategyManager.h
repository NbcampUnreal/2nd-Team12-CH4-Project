// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityStrategyManager.generated.h"

class UAbilityStrategy;
/**
 * 
 */
UCLASS()
class DFPROJECT_API UAbilityStrategyManager : public UObject
{
	GENERATED_BODY()
public:
	void RegisterAbility(FName AbilityName, UAbilityStrategy* Ability);

	void StartAbility(FName AbilityName, AActor* TargetActor);

	void StopAbility(FName AbilityName, AActor* TargetActor);

	bool IsAbilityActive(FName AbilityName) const;

	void ClearAllAbilities();

protected:
	UPROPERTY()
	TMap<FName, TObjectPtr<UAbilityStrategy>> RegisteredAbilities;
};
