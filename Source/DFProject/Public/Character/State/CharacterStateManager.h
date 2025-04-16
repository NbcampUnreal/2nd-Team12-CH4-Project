// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStateManager.generated.h"


enum class ECharacterStateType : uint8;
class UCharacterStateBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DFPROJECT_API UCharacterStateManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterStateManager();

	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetState(UCharacterStateBase* NewState);

	UFUNCTION(BlueprintCallable)
	bool IsCurrentState(ECharacterStateType StateType) const;

	UPROPERTY(ReplicatedUsing= OnRep_StateType)
	ECharacterStateType CurrentStateType;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCharacterStateBase> CurrentState;

protected:
	UFUNCTION()
	void OnRep_StateType();

	void SetStateByType(ECharacterStateType NewState);
};
