// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Grab/GrabComponent.h"

#include "Ability/Grab/Grabbable.h"
#include "Ability/Grab/GrabMover.h"

// Sets default values for this component's properties
UGrabComponent::UGrabComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UGrabComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UGrabComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsTryingToGrab || bIsGrabbing) return;

	DetectClosestGrabbable();

	if (CurrentTarget && GrabMover)
	{
		GrabMover->MoveTowardGrabTarget(CurrentTargetLocation);
	}
}

void UGrabComponent::DetectClosestGrabbable()
{
	TArray<FHitResult> Hits;
	FVector Start = ComputeDetectionStart();
	FVector End = ComputeDetectionEnd();

	FCollisionShape Shape = FCollisionShape::MakeSphere(DetectionRadius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		Shape,
		Params
	);

	AActor* ClosestActor = nullptr;
	float ClosestDistanceSq = FLT_MAX;

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || !IsValidGrabTarget(HitActor)) continue;

		float DistSq = FVector::DistSquared(Hit.ImpactPoint, GetOwner()->GetActorLocation());
		if (DistSq < ClosestDistanceSq)
		{
			ClosestDistanceSq = DistSq;
			ClosestActor = HitActor;
			CurrentTargetLocation = Hit.ImpactPoint;
		}
	}

	CurrentTarget = ClosestActor;

#if WITH_EDITOR
	DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, 0.1f);
	DrawDebugSphere(GetWorld(), CurrentTargetLocation, 10.f, 12, FColor::Red, false, 0.1f);
#endif
}

void UGrabComponent::StartGrab()
{
	bIsTryingToGrab = true;
	bIsGrabbing = false;
	CurrentTarget = nullptr;
}

void UGrabComponent::StopGrab()
{
	bIsTryingToGrab = false;
	bIsGrabbing = false;
	CurrentTarget = nullptr;

	if (GrabMover) GrabMover->ReleaseGrab();
}

bool UGrabComponent::IsValidGrabTarget(AActor* Actor) const
{
	return Actor->GetClass()->ImplementsInterface(UGrabbable::StaticClass());
}


FVector UGrabComponent::ComputeDetectionStart() const
{
	if (!GetOwner()) return FVector::ZeroVector;
	return GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 20.f; // 오프셋
}

FVector UGrabComponent::ComputeDetectionEnd() const
{
	if (!GetOwner()) return FVector::ZeroVector;
	return ComputeDetectionStart() + GetOwner()->GetActorForwardVector() * DetectionDistance;
}

void UGrabComponent::OnColliderOverlap(AActor* OverlappedActor)
{
	if (!bIsTryingToGrab || bIsGrabbing) return;
	if (OverlappedActor != CurrentTarget) return;

	if (GrabMover)
	{
		GrabMover->ExecuteGrab(OverlappedActor);
		bIsGrabbing = true;
	}
}

void UGrabComponent::SetGrabMover(TScriptInterface<IGrabMover> InGrabMover)
{
	GrabMover = InGrabMover;
}
