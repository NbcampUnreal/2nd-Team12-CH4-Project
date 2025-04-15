// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/Strategy/AbilityStrategy.h"
#include "Character/BodyPart/BodyPart.h"
#include "BodyPartAbilityStrategy.generated.h"

/**
 * 
 */
UCLASS()
class DFPROJECT_API UBodyPartAbilityStrategy : public UAbilityStrategy
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	void BeginOverlapEvent(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	virtual bool CanActivateAbility_Implementation(AActor* TargetActor) override;
	
	virtual void StartAbility_Implementation(AActor* TargetActor) override;
	
	virtual void EndAbility_Implementation(AActor* TargetActor) override;

protected:
	virtual float ComputeDamage(float ImpactForce);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability")
	UCurveFloat* DamageCurve;
	
	FTimerHandle AbilityEndTimerHandle;

	UPROPERTY(EditAnywhere, Category = Ability)
	float BaseDamage = 0;
	
	UPROPERTY(EditAnywhere, Category = "Ability")
	float AttackValidDuration = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Ability")
	float VirtualMass = 5.f;

	UPROPERTY(EditAnywhere, Category = "Ability")
	float ImpulsePower = 10000.0f;
	
	UPROPERTY()
	TWeakObjectPtr<ABodyPart> OwningBodyPart;

	UPROPERTY()
	TWeakObjectPtr<ACharacter> BodyPartOwner;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> DamagedActor;
};

