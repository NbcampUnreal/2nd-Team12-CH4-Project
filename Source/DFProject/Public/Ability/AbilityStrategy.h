// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityStrategy.generated.h"

class ABodyPart;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class DFPROJECT_API UAbilityStrategy : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	void StartAbility(ABodyPart* BodyPart);

	UFUNCTION(BlueprintNativeEvent)
	void StopAbility(ABodyPart* BodyPart);

	UFUNCTION(BlueprintNativeEvent)
	bool IsAbilityActive() const;
};
