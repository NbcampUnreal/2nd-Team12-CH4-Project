// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/State/CharacterStateManager.h"

#include "Character/DFCharacter.h"
#include "Character/State/CharacterStateBase.h"

// Sets default values for this component's properties
UCharacterStateManager::UCharacterStateManager()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UCharacterStateManager::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UCharacterStateManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (CurrentState) CurrentState->Tick(this->GetOwner<ADFCharacter>(), DeltaTime);
}

void UCharacterStateManager::SetState(UCharacterStateBase* NewState)
{
	if (!NewState) return;
	
	if (CurrentState) CurrentState->Exit(this->GetOwner<ADFCharacter>());

	CurrentState = NewState;

	CurrentState->Enter(this->GetOwner<ADFCharacter>());
}

