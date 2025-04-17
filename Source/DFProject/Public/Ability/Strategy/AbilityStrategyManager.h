// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityStrategyManager.generated.h"

class UAbilityStrategy;
/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DFPROJECT_API UAbilityStrategyManager : public UActorComponent
{
	GENERATED_BODY()
public:
	void RegisterAbility(FName AbilityName, UAbilityStrategy* Ability);

	void RemoveAbility(FName AbilityName);
	
	void StartAbility(FName AbilityName, AActor* TargetActor);

	void StopAbility(FName AbilityName, AActor* TargetActor);

	bool IsAbilityActive(FName AbilityName) const;

	void ClearAllAbilities();
	
	void InitializeAbilities();
	
	UPROPERTY(EditAnywhere, Category=Ability)
	TMap<FName, TSubclassOf<UAbilityStrategy>> InitialAbilities;
protected:
	
	UPROPERTY()
	TMap<FName, TObjectPtr<UAbilityStrategy>> RegisteredAbilities;
};
