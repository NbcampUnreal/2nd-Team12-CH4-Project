// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/State/IdleState.h"
#include "Character/DFCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UIdleState::Enter(ADFCharacter* Character)
{
	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	auto SMesh = Character->GetMesh();

	SMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	SMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	Character->GetCharacterMovement()->StopMovementImmediately();
	SMesh->SetSimulatePhysics(false);
	
	Character->PhysicalAnimComp->SetStrengthMultiplyer(0.5f);
	SMesh->SetRelativeTransform(Character->MeshOffset);
	SMesh->SetAllBodiesBelowSimulatePhysics(Character->PhysicalAnimStartBone, true, false);
	
	SMesh->bPauseAnims = false;
}

void UIdleState::Exit(ADFCharacter* Character)
{
}

void UIdleState::Tick(ADFCharacter* Character, float DeltaTime)
{
}
