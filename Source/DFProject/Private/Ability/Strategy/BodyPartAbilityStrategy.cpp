// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Strategy/BodyPartAbilityStrategy.h"

#include "Character/DFCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UBodyPartAbilityStrategy::BeginOverlapEvent_Implementation(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult)
{	
	if (!OtherActor ||
		OtherActor == OwningBodyPart->GetOwner() ||
		OtherActor->GetOwner() == OwningBodyPart->GetOwner()
		)
		return;
	
	if (UPrimitiveComponent* HitComp = Cast<UPrimitiveComponent>(OtherComp))
	{
		// 충돌 방향
		FVector Dir = (OtherActor->GetActorLocation() - OwningBodyPart->GetActorLocation()).GetSafeNormal();
		ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);

		if (!HitCharacter)
		{
			// 바디 파츠일 경우, OwningCharacter를 대신 참조
			if (ABodyPart* OtherBodyPart = Cast<ABodyPart>(OtherActor))
			{
				HitCharacter = OtherBodyPart->GetOwningCharacter();
			}
		}
		
		if (HitCharacter && !DamagedActor.Contains(HitCharacter))
		{
			// 공격자의 속도 기반으로 충격량 구해 추가 데미지 부여 (속도 * 질량)
			FVector Velocity = OwningBodyPart->GetBodyCollider()->GetComponentVelocity();
			float ImpactForce = Velocity.Size() * VirtualMass;
			float ImpactDamage = ComputeDamage(ImpactForce);

			UE_LOG(LogDamaged, Log, TEXT("ImpactForce: %f"), ImpactForce);
			
			UGameplayStatics::ApplyDamage(
				HitCharacter,
				BaseDamage + ImpactDamage,
				BodyPartOwner->GetController(),
				BodyPartOwner.Get(),
				UDamageType::StaticClass()
			);

			DamagedActor.Add(HitCharacter);
		}
	}
}

bool UBodyPartAbilityStrategy::CanActivateAbility_Implementation(AActor* TargetActor)
{
	if (!Super::CanActivateAbility_Implementation(TargetActor)) return false;

	ABodyPart* BodyPart = Cast<ABodyPart>(TargetActor);
	if (!BodyPart) EndAbility(TargetActor);
	OwningBodyPart = BodyPart;
	BodyPartOwner = OwningBodyPart->GetOwningCharacter();
	
	return true;
}

void UBodyPartAbilityStrategy::StartAbility_Implementation(AActor* TargetActor)
{
	if (!CanActivateAbility_Implementation(TargetActor)) return;
	
	if (OwningBodyPart->GetBodyCollider())
	{
		auto Collider = OwningBodyPart->GetBodyCollider();
		if (!Collider->OnComponentBeginOverlap.IsAlreadyBound(this, &UBodyPartAbilityStrategy::BeginOverlapEvent))
		{
			Collider->OnComponentBeginOverlap.AddDynamic(this, &UBodyPartAbilityStrategy::BeginOverlapEvent);
		}
	}
	
	bIsAbilityActive = true;
	LastUsedTime = GetWorld()->GetTimeSeconds();
	
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("EndAbility"), TargetActor);
	GetWorld()->GetTimerManager().SetTimer(AbilityEndTimerHandle, TimerDelegate, AttackValidDuration, false);

	ActivateAbility_Implementation(TargetActor);
}

void UBodyPartAbilityStrategy::EndAbility_Implementation(AActor* TargetActor)
{
	Super::EndAbility_Implementation(TargetActor);
	
	if (OwningBodyPart.Get() && OwningBodyPart->GetBodyCollider())
	{
		OwningBodyPart->GetBodyCollider()->OnComponentBeginOverlap.RemoveDynamic(this, &UBodyPartAbilityStrategy::BeginOverlapEvent);
	}

	GetWorld()->GetTimerManager().ClearTimer(AbilityEndTimerHandle);

	OwningBodyPart = nullptr;
	BodyPartOwner = nullptr;
	DamagedActor.Empty();
}

float UBodyPartAbilityStrategy::ComputeDamage(float ImpactForce)
{
	return DamageCurve ? DamageCurve->GetFloatValue(ImpactForce) : 0.0f;
}
