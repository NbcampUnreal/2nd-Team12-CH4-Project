// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabHandler.generated.h"

class UPhysicsConstraintComponent;
class UGrabComponent;

USTRUCT(BlueprintType)
struct FGrabTargetInfo
{
	GENERATED_BODY()
	
	UPROPERTY()
	AActor* TargetActor = nullptr;

	UPROPERTY()
	UPrimitiveComponent* TargetComponent = nullptr;

	UPROPERTY()
	FVector HitLocation = FVector::ZeroVector;

	UPROPERTY()
	FVector HitNormal = FVector::ZeroVector;

	UPROPERTY()
	FName SocketName = NAME_None;

	// 소켓 근처라면 아이템 사용 가능하도록 bool값?
	UPROPERTY()
	bool CanUse = false;
};


/**
 * GrabComponent가 필터링과 감지, 흐름을 관리한다면
 * 여긴 감지한 대상을 어떻게 잡을 지, 잡고 난 후의 처리를 담당
 */
UCLASS(Blueprintable)
class UGrabHandler : public UObject
{
	GENERATED_BODY()
public:
	
	void SetOwningGrabComponent(UGrabComponent* GrabComp);
	bool IsGrabbable(const AActor* Target);
	
	UFUNCTION(BlueprintCallable, Category = "Grab")
	virtual void MoveToTarget(const FVector& Location) {}
	
	UFUNCTION(BlueprintCallable, Category = "Grab")
	virtual void ExecuteGrab(const FGrabTargetInfo& TargetInfo);
	
	UFUNCTION(BlueprintCallable, Category = "Grab")
	virtual void ReleaseGrab();

	UFUNCTION(BlueprintCallable, Category = "Grab")
	virtual void OverlapGrabTarget(const FGrabTargetInfo& TargetInfo);

protected:
	UFUNCTION()
	void OnGrabColliderBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);
	
	UFUNCTION()
	void OnGrabColliderHit(UPrimitiveComponent* HitComponent,
					   AActor* OtherActor,
					   UPrimitiveComponent* OtherComp,
					   FVector NormalImpulse,
					   const FHitResult& Hit);
	UPROPERTY()
	UGrabComponent* OwningGrabComponent;

};