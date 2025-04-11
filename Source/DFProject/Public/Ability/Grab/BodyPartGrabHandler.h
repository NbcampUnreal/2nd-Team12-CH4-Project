// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabHandler.h"
#include "Components/SphereComponent.h"
#include "BodyPartGrabHandler.generated.h"

class UPhysicsHandleComponent;
class ABodyPart;
class UPhysicsConstraintComponent;
/**
 * 
 */
UCLASS()
class DFPROJECT_API UBodyPartGrabHandler : public UGrabHandler
{
	GENERATED_BODY()
public:
	UBodyPartGrabHandler();

	virtual void BeginDestroy() override;

	void Initialize(ABodyPart* BodyPart);

	virtual void MoveToTarget(const FVector& TargetLocation) override;
	virtual void ExecuteGrab(const FGrabTargetInfo& TargetInfo) override;
	virtual void ReleaseGrab() override;
	
protected:
	
	UPROPERTY()
	TWeakObjectPtr<USceneComponent> Root;

	UPROPERTY()
	TWeakObjectPtr<USphereComponent> GrabCollider;

	UPROPERTY()
	TObjectPtr<UPhysicsConstraintComponent> GrabConstraint;
	
	UPROPERTY(EditAnywhere, Category = "Grab")
	float MoveForce = 500000.f;
	
	UPROPERTY()
	AActor* CurrentGrabTarget;
};
