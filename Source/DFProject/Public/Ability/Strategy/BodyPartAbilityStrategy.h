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

	virtual bool CanActivateAbility_Implementation(AActor* TargetActor) const override;
	
	virtual void StartAbility_Implementation(AActor* TargetActor) override;
	
	virtual void EndAbility_Implementation(AActor* TargetActor) override;

protected:
	FTimerHandle AbilityEndTimerHandle;
	
	UPROPERTY(EditAnywhere, Category = "Ability")
	float AttackValidDuration = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Ability")
	float VirtualMass = 30.f;

	UPROPERTY(EditAnywhere, Category = "Ability")
	float ImpulsePower = 10000.0f;
	
	UPROPERTY()
	TWeakObjectPtr<ABodyPart> OwningBodyPart;

	UPROPERTY()
	TWeakObjectPtr<ACharacter> BodyPartOwner;
};
