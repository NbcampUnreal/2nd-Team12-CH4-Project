// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterStateBase.h"
#include "StunnedState.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UStunnedState : public UCharacterStateBase
{
	GENERATED_BODY()
public:
	virtual void Enter(ADFCharacter* Character) override;
	virtual void Tick(ADFCharacter* Character, float DeltaTime) override;
	virtual void Exit(ADFCharacter* Character) override;
	virtual ECharacterStateType GetStateType() const override {return ECharacterStateType::Stunned;}

private:
};
