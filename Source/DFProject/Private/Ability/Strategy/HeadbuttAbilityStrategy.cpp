// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Strategy/HeadbuttAbilityStrategy.h"

#include "Character/BodyPart/BodyPart.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"


UHeadbuttAbilityStrategy::UHeadbuttAbilityStrategy()
{
	CoolTime = 0.3f;
	ImpulsePower = 50000.f;
	BaseDamage = 50.f;
}

void UHeadbuttAbilityStrategy::ActivateAbility_Implementation(AActor* TargetActor)
{
	FVector ImpulseDirection = BodyPartOwner->GetActorForwardVector() * ImpulsePower; // 방향도 매개변수로 받을까 고민
	OwningBodyPart->GetBodyCollider()->AddImpulse(ImpulseDirection, NAME_None, true);
}
