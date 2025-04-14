// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Strategy/PunchAbilityStrategy.h"

#include "Character/DFCharacter.h"
#include "Character/BodyPart/BodyPart.h"
#include "Components/SphereComponent.h"

UPunchAbilityStrategy::UPunchAbilityStrategy()
{
	CoolTime = 0.3f;
	ImpulsePower = 30000.f;
}

void UPunchAbilityStrategy::ActivateAbility_Implementation(AActor* TargetActor)
{
	FVector ImpulseDirection = BodyPartOwner->GetActorForwardVector() * ImpulsePower;
	OwningBodyPart->GetBodyCollider()->AddImpulse(ImpulseDirection, NAME_None, true);
}
