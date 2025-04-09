// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabMover.generated.h"

/**
 * 
 */
UINTERFACE(Blueprintable)
class UGrabMover : public UInterface
{
	GENERATED_BODY()
};

class IGrabMover
{
	GENERATED_BODY()

public:
	virtual void MoveTowardGrabTarget(const FVector& TargetLocation) = 0;
	virtual void ExecuteGrab(AActor* Target) = 0;
	virtual void ReleaseGrab() = 0;
};