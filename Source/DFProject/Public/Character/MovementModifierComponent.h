// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MovementModifierComponent.generated.h"

struct FGrabTargetInfo;

USTRUCT()
struct FTimedForce
{
	GENERATED_BODY()

	float Duration;
	FVector Force;
};

USTRUCT()
struct FGrabInteractionData
{
	GENERATED_BODY()

	TWeakObjectPtr<UObject> Target;
	float LastKnownForce;
	int Ref = 0;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DFPROJECT_API UMovementModifierComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMovementModifierComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "MovementModifier")
	void AddEnvironmentalForce(const FVector& Force, float Duration);

	UFUNCTION(BlueprintCallable, Category="MovementModifier")
	void RegisterGrabInteraction(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category="MovementModifier")
	void UnregisterGrabInteraction(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementModifier")
	bool bApplyGrabResistance = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementModifier")
	bool bApplyEnvironmentalForces = true;
private:

	FVector CalculateTotalForce();
	FVector CalculateEnvironmentalForces();
	FVector CalculateGrabbedResistance();

	UPROPERTY()
	TArray<FTimedForce> EnvironmentalForces;
	
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, FGrabInteractionData> ActiveGrabs; // body part 에 Grabbable 빼고 캐릭터에 붙이기?
};

