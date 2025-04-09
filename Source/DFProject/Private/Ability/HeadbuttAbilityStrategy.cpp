// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/HeadbuttAbilityStrategy.h"

#include "Character/BodyPart/BodyPart.h"

void UHeadbuttAbilityStrategy::StartAbility_Implementation(ABodyPart* BodyPart)
{
	if (!BodyPart) return;

	BodyPart->SaveAttackTime();
	BodyPart->ApplyImpulse();
}
