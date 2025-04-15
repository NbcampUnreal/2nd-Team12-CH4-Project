// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BodyPartAbilityStrategy.h"
#include "HeadbuttAbilityStrategy.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UHeadbuttAbilityStrategy : public UBodyPartAbilityStrategy
{
	GENERATED_BODY()
public:
	UHeadbuttAbilityStrategy();
	virtual void ActivateAbility_Implementation(AActor* TargetActor) override;
};
