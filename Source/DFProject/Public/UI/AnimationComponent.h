// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "LatentActions.h"
#include "AnimationComponent.generated.h"

class FAnimationLatentAction : public FPendingLatentAction
{
public:
	bool bIsDone = false;
	FLatentActionInfo Info;

	FAnimationLatentAction(const FLatentActionInfo& InInfo)
		: Info(InInfo) {
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		Response.FinishAndTriggerIf(bIsDone, Info.ExecutionFunction, Info.Linkage, Info.CallbackTarget);
	}
};

UENUM(BlueprintType)
enum class EAnimationPlayMode : uint8
{
	Play				UMETA(DisplayName = "Play"),
	Reverse			    UMETA(DisplayName = "Reverse"),
	Stop			    UMETA(DisplayName = "Stop"),
	Reset			    UMETA(DisplayName = "Reset")
};

USTRUCT(BlueprintType)
struct FAnimationData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Animation")
	bool bUseLocalCoord = true;

	UPROPERTY(EditAnywhere, Category = "Animation")
	float Distance = 0.f;

	UPROPERTY(EditAnywhere, Category = "Animation")
	float StopDelay = 0.f;

	UPROPERTY(EditAnywhere, Category = "Animation")
	FTransform TransformOffset;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UCurveFloat* Curve = nullptr;

	UPROPERTY(EditAnywhere, Category = "Animation")
	EAnimationPlayMode PlayMode = EAnimationPlayMode::Play;
};

USTRUCT(BlueprintType)
struct FAnimationSequence
{
	GENERATED_BODY()

	int32 CurrentStepIndex = 0;

	UPROPERTY(EditAnywhere, Category = "Animation")
	TArray<FAnimationData> Steps;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DFPROJECT_API UAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAnimationComponent();

protected:
	virtual void BeginPlay() override;

protected:
	/* component */
	UPROPERTY()
	UTimelineComponent* TimelineComp;

	/* can edit variables on bluprint */
	UPROPERTY(EditAnywhere, Category = "Animation")
	TMap<FName, FAnimationSequence> AnimationMap;

	/* cached data */
	FTransform InitTransform;
	FTransform CachedTransform;
	FTransform StartTransform;
	FTransform TargetTransform;
	FName CurrentEvent;
	AActor* OwnerActor;

	/* timeline variables */
	float CurrentPlaybackPos = 0.f;
	FOnTimelineFloat Progress;
	FOnTimelineEvent Finished;

	/* Timer */
	FTimerHandle TimerHandle;

	/* Latent Variable */
	FLatentActionInfo CurrentLatentInfo;

	/* transform method */
	void SaveActorTransform(const FAnimationData& Data);
	void CalculateTransform(FTransform& Base, const FTransform& Offset);
	void InvertTransform(FTransform& Transform);

	/* animation logic */
	UFUNCTION()
	void OnTimelineUpdate(float Value);

	/* animation ended */
	UFUNCTION()
	void OnTimelineFinished();

	/* animation step */
	void PlayStep(FAnimationData& Data);

public:
	/* event start */
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContextObject"))
	void PlayEvent(UObject* WorldContextObject, FLatentActionInfo LatentInfo, const FName EventName);
protected:
	void EndEvent();

};
