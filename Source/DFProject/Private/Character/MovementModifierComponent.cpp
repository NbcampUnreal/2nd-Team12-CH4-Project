// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementModifierComponent.h"

#include "Ability/Grab/Grabbable.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UMovementModifierComponent::UMovementModifierComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMovementModifierComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector TotalForce = CalculateTotalForce();

	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement())
		{
			MoveComp->Velocity += TotalForce * DeltaTime;
			
		}
	}
}

void UMovementModifierComponent::AddEnvironmentalForce(const FVector& Force, float Duration)
{
	FTimedForce NewForce;
	NewForce.Force = Force;
	NewForce.Duration = Duration;
	EnvironmentalForces.Add(NewForce);
}

void UMovementModifierComponent::RegisterGrabInteraction(AActor* TargetActor)
{
	if (!TargetActor || !TargetActor->GetClass()->ImplementsInterface(UGrabbable::StaticClass()))
	{
		return;
	}

	AActor* ActualTarget = IGrabbable::Execute_GetActualTarget(TargetActor);
	if (!ActualTarget) return;

	if (ActiveGrabs.Contains(ActualTarget))
	{
		ActiveGrabs[ActualTarget].Ref++;
	}
	else
	{
		ActiveGrabs.Add(ActualTarget);
	}
}

void UMovementModifierComponent::UnregisterGrabInteraction(AActor* TargetActor)
{
	if (!TargetActor || !TargetActor->GetClass()->ImplementsInterface(UGrabbable::StaticClass()))
	{
		return;
	}

	AActor* ActualTarget = IGrabbable::Execute_GetActualTarget(TargetActor);
	if (!ActualTarget) return;

	if (!ActiveGrabs.Contains(ActualTarget))
	{
		return;
	}

	FGrabInteractionData& InteractionData = ActiveGrabs[ActualTarget];

	if (InteractionData.Ref > 1)
	{
		InteractionData.Ref--;
	}
	else
	{
		ActiveGrabs.Remove(ActualTarget);
	}
}

FVector UMovementModifierComponent::CalculateTotalForce()
{
	FVector TotalForce = FVector::ZeroVector;

	if (bApplyEnvironmentalForces)
	{
		TotalForce += CalculateEnvironmentalForces();
	}

	if (bApplyGrabResistance)
	{
		TotalForce += CalculateGrabbedResistance();
	}
	
	return TotalForce;
}

FVector UMovementModifierComponent::CalculateGrabbedResistance()
{
	FVector TotalResistance = FVector::ZeroVector;

	for (auto& Grab : ActiveGrabs)
	{
		AActor* Target = Grab.Key.Get();
		FVector Resistance = IGrabbable::Execute_GetResistanceForce(Target, GetOwner());

		TotalResistance += Resistance;
	}

	return TotalResistance;
}

FVector UMovementModifierComponent::CalculateEnvironmentalForces()
{
	FVector AccumulatedForce = FVector::ZeroVector;

	for (int32 i = EnvironmentalForces.Num() - 1; i >= 0; --i)
	{
		AccumulatedForce += EnvironmentalForces[i].Force;
		EnvironmentalForces[i].Duration -= GetWorld()->GetDeltaSeconds();

		if (EnvironmentalForces[i].Duration <= 0)
		{
			EnvironmentalForces.RemoveAt(i);
		}
	}

	return AccumulatedForce;
}