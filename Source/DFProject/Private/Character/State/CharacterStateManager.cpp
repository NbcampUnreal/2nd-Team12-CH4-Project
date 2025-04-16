// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/State/CharacterStateManager.h"

#include "Character/DFCharacter.h"
#include "Character/State/CharacterStateBase.h"
#include "Character/State/GrabbedState.h"
#include "Character/State/IdleState.h"
#include "Character/State/RecoverState.h"
#include "Character/State/StunnedState.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCharacterStateManager::UCharacterStateManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterStateManager::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);
}

void UCharacterStateManager::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterStateManager, CurrentStateType);
}

void UCharacterStateManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (CurrentState) CurrentState->Tick(this->GetOwner<ADFCharacter>(), DeltaTime);
}

void UCharacterStateManager::SetState(UCharacterStateBase* NewState)
{
	if (!NewState || !GetOwner()->HasAuthority()) return;
	
	if (CurrentState) CurrentState->Exit(this->GetOwner<ADFCharacter>());
	
	CurrentState = NewState;
	CurrentStateType = CurrentState->GetStateType();
	
	CurrentState->Enter(this->GetOwner<ADFCharacter>());
}

bool UCharacterStateManager::IsCurrentState(ECharacterStateType StateType) const
{
	return CurrentStateType == StateType;
}

void UCharacterStateManager::OnRep_StateType()
{
	SetStateByType(CurrentStateType);
}

void UCharacterStateManager::SetStateByType(ECharacterStateType NewState)
{
	ADFCharacter* Character = Cast<ADFCharacter>(GetOwner());
	if (!Character) return;
	
	if (CurrentState)
	{
		CurrentState->Exit(Character);
	}

	switch (NewState)
	{
	case ECharacterStateType::Grabbed:
		CurrentState = NewObject<UGrabbedState>(this);
		break;
	case ECharacterStateType::Recover:
		CurrentState = NewObject<URecoverState>(this);
		break;
	case ECharacterStateType::Stunned:
		CurrentState = NewObject<UStunnedState>(this);
		break;
	//case ECharacterStateType::Dead:
	//	CurrentState = NewObject<UDeadState>(this);
	//	break;
	case ECharacterStateType::Idle:
	default:
		CurrentState = NewObject<UIdleState>(this);
		break;
	}

	if (CurrentState)
	{
		CurrentState->Enter(Character);
	}
}

