// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStateManager.generated.h"


class UCharacterStateBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DFPROJECT_API UCharacterStateManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterStateManager();

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetState(UCharacterStateBase* NewState);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCharacterStateBase> CurrentState;
};
