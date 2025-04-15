// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityStrategy.generated.h"

enum class ECharacterStateType : uint8;
class ABodyPart;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class DFPROJECT_API UAbilityStrategy : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartAbility(AActor* TargetActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ActivateAbility(AActor* TargetActor);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndAbility(AActor* TargetActor);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanActivateAbility(AActor* TargetActor);

	UPROPERTY(EditAnywhere, Category="Ability")
	TArray<ECharacterStateType> AllowedStates;
	
	UPROPERTY(EditAnywhere, Category="Ability")
	float CoolTime = 0;
	
	UPROPERTY(BlueprintReadOnly)
	float LastUsedTime = 0;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsAbilityActive = false;
};
