// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grabbable.generated.h"

class UPhysicsConstraintComponent;

UENUM(BlueprintType)
enum class EGrabMode : uint8
{
	LooseAttach      UMETA(DisplayName = "Loose Attach"),      // 단순히 잡기만
	AttachOnly       UMETA(DisplayName = "Attach Only"),       // 소켓에 붙지만 활성화 없음
	AttachAndEnable  UMETA(DisplayName = "Attach and Enable")  // 소켓에 붙고, 기능 활성화
};

UINTERFACE(MinimalAPI, Blueprintable)
class UGrabbable : public UInterface
{
	GENERATED_BODY()
};

class DFPROJECT_API IGrabbable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable")
	TArray<FName> GetGrabSocketNames() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable")
	void DestroyThis();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable")
	AActor* GetActualTarget();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable")
	void OnGrabbed(AActor* TargetActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable")
	void OnGrabbedBy(AActor* Grabber, UPhysicsConstraintComponent* GrabbersConstraint);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable")
	void OnGrabReleased(AActor* TargetActor);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable")
	void OnGrabReleasedBy(AActor* Grabber);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Grabbable")
	FVector GetResistanceForce(AActor* PullingActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Grabbable")
	UPrimitiveComponent* GetRoot();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Grabbable")
	void Use();

	TWeakObjectPtr<UPhysicsConstraintComponent> GrabberConstraint; // 아직은 단일 대상만 생각
};