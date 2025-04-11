// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStateComponent.generated.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	None = 0,
	Stunned = 1 << 0,
	Recovering = 1 << 1,
	Grabbing = 1 << 2,
	Dead = 1 << 3,
};
ENUM_CLASS_FLAGS(ECharacterState)


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DFPROJECT_API UCharacterStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	void AddState(ECharacterState NewState);
	void RemoveState(ECharacterState StateToRemove);
	bool IsInState(ECharacterState StateToCheck) const;

private:
	ECharacterState CurrentState = ECharacterState::None;
};
