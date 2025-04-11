// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrabMovementModifier.generated.h"


class UCharacterMovementComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DFPROJECT_API UGrabMovementModifier : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabMovementModifier();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ActivateModifier(AActor* InTargetActor);
	void DeactivateModifier();

private:
	TWeakObjectPtr<AActor> TargetActor;

	float PullStrength;

	UPROPERTY(EditAnywhere)
	bool bEnableDebugDraw = true;

	UPROPERTY(EditAnywhere)
	float InterpSpeed = 5.f;

	float CurrentInterpolatedStrength;

	float CalculateStrength(UCharacterMovementComponent* MoveComp) const;
};
