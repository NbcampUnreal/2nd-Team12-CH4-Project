// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Strategy/PunchAbilityStrategy.h"

#include "Character/DFCharacter.h"
#include "Character/BodyPart/BodyPart.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

UPunchAbilityStrategy::UPunchAbilityStrategy()
{
	CoolTime = 0.3f;
	ImpulsePower = 30000.f;
	BaseDamage = 10.f;
}

void UPunchAbilityStrategy::ActivateAbility_Implementation(AActor* TargetActor)
{
	FVector Impulse = BodyPartOwner->GetActorForwardVector() * ImpulsePower;
	OwningBodyPart->Multicast_AddImpulse(Impulse);
	if (AttackSound) UGameplayStatics::PlaySoundAtLocation(this, AttackSound, OwningBodyPart->GetActorLocation());
}
