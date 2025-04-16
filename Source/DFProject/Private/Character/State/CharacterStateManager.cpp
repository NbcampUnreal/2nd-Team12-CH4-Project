// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/State/CharacterStateManager.h"

#include "Character/DFCharacter.h"
#include "Character/State/CharacterStateBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCharacterStateManager::UCharacterStateManager()
{
	PrimaryComponentTick.bCanEverTick = true;
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
	//switch (CurrentStateType)
	//{
	//case ECharacterStateType::Stunned:
	//	// 클라에서 스턴 이펙트 재생
	//		break;
	//case ECharacterStateType::Dead:
	//
	//		break;
	//case ECharacterStateType::Idle:
	//default:
	//	// 기본 상태 처리
	//	break;
	//}
}

