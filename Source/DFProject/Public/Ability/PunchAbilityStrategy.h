// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/AbilityStrategy.h"
#include "PunchAbilityStrategy.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UPunchAbilityStrategy : public UAbilityStrategy
{
	GENERATED_BODY()
public:
	virtual void StartAbility_Implementation(ABodyPart* BodyPart) override;
};
