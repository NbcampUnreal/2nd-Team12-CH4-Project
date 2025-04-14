// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BodyPartAbilityStrategy.h"
#include "PunchAbilityStrategy.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UPunchAbilityStrategy : public UBodyPartAbilityStrategy
{
	GENERATED_BODY()
public:
	UPunchAbilityStrategy();
	virtual void ActivateAbility_Implementation(AActor* TargetActor) override;
	virtual void EndAbility_Implementation(AActor* TargetActor) override;
};

inline void UPunchAbilityStrategy::EndAbility_Implementation(AActor* TargetActor)
{
	Super::EndAbility_Implementation(TargetActor);
}
