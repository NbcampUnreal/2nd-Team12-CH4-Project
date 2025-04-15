// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/State/CharacterStateBase.h"
#include "RecoverState.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API URecoverState : public UCharacterStateBase
{
	GENERATED_BODY()
public:
	virtual void Enter(ADFCharacter* Character) override;
	virtual void Tick(ADFCharacter* Character, float DeltaTime) override;
	virtual void Exit(ADFCharacter* Character) override;
	virtual ECharacterStateType GetStateType() const override {return ECharacterStateType::Recover;}

	virtual bool CanChangeToState(ECharacterStateType NewState) override;
private:
	UPROPERTY()
	FQuat InitialRecoveryRotation;
	
	float RecoverAlpha = 0.0f;
	float RecoverSpeed = 0.8f;
	bool bRecoverFinished = false;
};

