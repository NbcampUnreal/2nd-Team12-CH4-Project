// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/State/CharacterStateBase.h"
#include "GrabbedState.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UGrabbedState : public UCharacterStateBase
{
	GENERATED_BODY()
public:
	virtual void Enter(ADFCharacter* Character) override;
	virtual void Tick(ADFCharacter* Character, float DeltaTime) override;
	virtual void Exit(ADFCharacter* Character) override;
	virtual ECharacterStateType GetStateType() const override {return ECharacterStateType::Grabbed;}
};
