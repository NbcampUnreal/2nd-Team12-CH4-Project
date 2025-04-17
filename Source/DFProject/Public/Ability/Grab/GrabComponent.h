// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabHandler.h"
#include "Components/ActorComponent.h"
#include "GrabComponent.generated.h"

enum class EBodyPartType : uint8;
class UBodyPartGrabHandler;
class UPhysicsConstraintComponent;
class IGrabHandler;
class ABodyPart;

UENUM(BlueprintType)
enum class EGrabState : uint8
{
	Idle,
	Detecting,
	MovingToGrab,
	Grabbing,
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrabStart, const FGrabTargetInfo&, TargetInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrabRelease, const FGrabTargetInfo&, TargetInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrabFailed, const FGrabTargetInfo&, TargetInfo);

/**
 * 필터링과 감지, 흐름을 관리
 * 
 * Handler를 선택해 감지한 대상을 어떻게 잡을 지 처리 가능
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DFPROJECT_API UGrabComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGrabComponent();
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartGrab(); // 소유자가 잡기를 시작할 때 호출

	void StopGrab(); // 잡기 감지 취소

	UFUNCTION(BlueprintCallable, Category = "GrabEvents") // 대상이 잡힐 때 호출됨.
	void Grabbed(const FGrabTargetInfo& Info);
	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "GrabEvents") // 대상이 잡힐 때 호출됨.
	void Multicast_Grabbed(const FGrabTargetInfo& Info);
	
	UFUNCTION(BlueprintCallable, Category = "GrabEvents") // 대상을 놓을 때 호출해야함.
	void Released();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "GrabEvents") // 대상이 잡힐 때 호출됨.
	void Multicast_Released();

	UFUNCTION(NetMulticast, Reliable, Category = "GrabEvents")
	void InitializeGrabHandler(EBodyPartType Type);

	void SetGrabState(EGrabState NewState);

	EGrabState GetCurrentGrabState();

	AActor* GetGrabTargetActor();
	
	UPROPERTY(BlueprintAssignable, Category = "GrabEvents")
	FOnGrabStart OnGrabbed;

	UPROPERTY(BlueprintAssignable, Category = "GrabEvents")
	FOnGrabRelease OnGrabRelease;

	UPROPERTY(BlueprintAssignable, Category = "GrabEvents")
	FOnGrabFailed OnGrabFailed;

	bool HasValidHandler() const;

	UBodyPartGrabHandler* GetGrabHandler() const;
	void CreateHandler(ABodyPart* BodyPart);
protected:
	void DetectClosestGrabbable();
	
	bool IsValidGrabTarget(AActor* Actor) const;

	FVector ComputeDetectionStart() const;
	FVector ComputeDetectionEnd() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RequiredSocketName;
	
	UPROPERTY()
	AActor* CurrentTarget;

	UPROPERTY()
	FVector CurrentTargetLocation;
	
	UPROPERTY()
	TObjectPtr<UBodyPartGrabHandler> GrabHandler;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DetectionRadius = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetectionDistance = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetectionAngle;

	EGrabState CurrentState = EGrabState::Idle;

	FGrabTargetInfo GrabbedTargetInfo;

	TObjectPtr<UPhysicsConstraintComponent> GrabConstraint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float FrontOffset = 30.0f;
};

