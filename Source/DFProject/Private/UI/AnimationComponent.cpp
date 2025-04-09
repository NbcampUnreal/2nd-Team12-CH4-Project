// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AnimationComponent.h"
#include "Kismet/KismetMathLibrary.h"

UAnimationComponent::UAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	TimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	TimelineComp->RegisterComponent();
}

void UAnimationComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = GetOwner();

	if (OwnerActor)
	{
		InitTransform = CachedTransform = OwnerActor->GetTransform();
	}

	Progress.BindUFunction(this, FName("OnTimelineUpdate"));
	Finished.BindUFunction(this, FName("OnTimelineFinished"));

	TimelineComp->SetTimelineFinishedFunc(Finished);
}

void UAnimationComponent::SaveActorTransform(const FAnimationData& Data)
{
	if (!OwnerActor) return;
	FTransform Offset = Data.TransformOffset;

	StartTransform = TargetTransform = OwnerActor->GetTransform();

	if (Data.bUseLocalCoord)
	{
		FVector Forward = StartTransform.GetRotation().GetForwardVector();
		Offset.SetLocation(Forward * Data.Distance);
	}

	if (Data.PlayMode == EAnimationPlayMode::Reverse)
	{
		InvertTransform(Offset);
		CalculateTransform(StartTransform, Offset);
	}
	else
	{
		CalculateTransform(TargetTransform, Offset);
	}
}

void UAnimationComponent::CalculateTransform(FTransform& Base, const FTransform& Offset)
{
	Base.AddToTranslation(Offset.GetLocation());
	Base.ConcatenateRotation(Offset.GetRotation());
	Base.SetScale3D(Base.GetScale3D() * Offset.GetScale3D());
}

void UAnimationComponent::InvertTransform(FTransform& Transform)
{
	Transform.SetLocation(Transform.GetLocation() * -1.f);

	FRotator Rot = Transform.GetRotation().Rotator();
	Transform.SetRotation(FQuat(FRotator(-Rot.Pitch, -Rot.Yaw, -Rot.Roll)));

	FVector Scale = Transform.GetScale3D();
	Scale = FVector(
		Scale.X != 0.f ? 1.f / Scale.X : 1.f,
		Scale.Y != 0.f ? 1.f / Scale.Y : 1.f,
		Scale.Z != 0.f ? 1.f / Scale.Z : 1.f
	);
	Transform.SetScale3D(Scale);
}

void UAnimationComponent::OnTimelineUpdate(float Value)
{
	if (!OwnerActor) return;

	FTransform Interp = UKismetMathLibrary::TLerp(StartTransform, TargetTransform, Value);
	OwnerActor->SetActorRelativeTransform(Interp);
}

void UAnimationComponent::OnTimelineFinished()
{
	if (!AnimationMap.Contains(CurrentEvent)) return;

	FAnimationSequence& Sequence = AnimationMap[CurrentEvent];

	Sequence.CurrentStepIndex++;

	if (Sequence.CurrentStepIndex >= Sequence.Steps.Num())
	{
		CachedTransform = OwnerActor->GetTransform();
	}

	if (Sequence.Steps.IsValidIndex(Sequence.CurrentStepIndex))
	{
		PlayStep(Sequence.Steps[Sequence.CurrentStepIndex]);
	}
}

void UAnimationComponent::PlayEvent(const FName EventName)
{
	if (!AnimationMap.Contains(EventName)) return;

	FAnimationSequence& Sequence = AnimationMap[EventName];
	Sequence.CurrentStepIndex = 0;

	OwnerActor->SetActorTransform(CachedTransform);

	if (Sequence.Steps.Num() == 0) return;

	CurrentEvent = EventName;
	PlayStep(Sequence.Steps[Sequence.CurrentStepIndex]);
}

void UAnimationComponent::PlayStep(FAnimationData& Data)
{
	if (!TimelineComp) return;
	if (Data.Curve)
	{
		TimelineComp->AddInterpFloat(Data.Curve, Progress);
	}

	SaveActorTransform(Data);

	TimelineComp->SetPlaybackPosition(0.f, false);
	TimelineComp->SetLooping(false);
	TimelineComp->SetPlayRate(1.f);
	TimelineComp->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

	switch (Data.PlayMode)
	{
	case EAnimationPlayMode::Play:
		TimelineComp->Play();
		break;

	case EAnimationPlayMode::Reverse:
		TimelineComp->SetPlaybackPosition(TimelineComp->GetTimelineLength(), false);
		TimelineComp->Reverse();
		break;

	case EAnimationPlayMode::Stop:

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UAnimationComponent::OnTimelineFinished, Data.StopDelay, false);
		break;

	case EAnimationPlayMode::Reset:
		if (OwnerActor)
		{
			OwnerActor->SetActorTransform(InitTransform);
		}
		OnTimelineFinished();
		break;
	}
}
