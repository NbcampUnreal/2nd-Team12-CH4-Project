// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrabComponent.generated.h"

class IGrabMover;
class ABodyPart;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DFPROJECT_API UGrabComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGrabComponent();

	virtual void BeginPlay() override;

	void StartGrab();

	void StopGrab();

	void OnColliderOverlap(AActor* OverlappedActor);

	void SetGrabMover(TScriptInterface<IGrabMover> InGrabMover);

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DetectClosestGrabbable();
	
	bool IsValidGrabTarget(AActor* Actor) const;

	FVector ComputeDetectionStart() const;
	FVector ComputeDetectionEnd() const;
	
	UPROPERTY()
	AActor* CurrentTarget;

	UPROPERTY()
	FVector CurrentTargetLocation;
	
	UPROPERTY()
	TScriptInterface<IGrabMover> GrabMover;

	bool bIsTryingToGrab = false;
	bool bIsGrabbing = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DetectionRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetectionDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetectionAngle;
};


