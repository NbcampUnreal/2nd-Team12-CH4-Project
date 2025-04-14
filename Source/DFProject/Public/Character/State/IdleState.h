// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterStateBase.h"
#include "IdleState.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UIdleState : public UCharacterStateBase
{
	GENERATED_BODY()
public:
	virtual void Enter(ADFCharacter* Character) override;
	virtual void Exit(ADFCharacter* Character) override;
	virtual void Tick(ADFCharacter* Character, float DeltaTime) override;
	virtual ECharacterStateType GetStateType() const override {return ECharacterStateType::Idle;}
};
