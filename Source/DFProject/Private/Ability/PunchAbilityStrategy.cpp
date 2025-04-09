// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/PunchAbilityStrategy.h"

#include "Character/BodyPart/BodyPart.h"

void UPunchAbilityStrategy::StartAbility_Implementation(ABodyPart* BodyPart)
{
	if (!BodyPart) return;

	BodyPart->SaveAttackTime();
	BodyPart->ApplyImpulse();
}
