// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Grab/GrabComponent.h"

#include "Ability/Grab/Grabbable.h"
#include "Ability/Grab/GrabHandler.h"
#include "Character/MovementModifierComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values for this component's properties
UGrabComponent::UGrabComponent()
{
	PrimaryComponentTick.bCanEverTick = true;     // Tick 가능하게 설정
	PrimaryComponentTick.bStartWithTickEnabled = true; // 시작 시 Tick 활성화

	bAutoActivate = true; // 자동 활성화
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

	switch (CurrentState)
	{
	case EGrabState::Idle:
		break;

	case EGrabState::Detecting:
		UE_LOG(LogTemp, Warning, TEXT("Grab Component is Detecting closest grabbable."));
		DetectClosestGrabbable();
		if (CurrentTarget && GrabHandler)
		{
			GrabHandler->MoveToTarget(CurrentTargetLocation); // 이동 상태로 진행
			// 타겟이 잡히면 Grabbing 상태로 전환
			//SetGrabState(EGrabState::Grabbing);
		}
		break;

	case EGrabState::Grabbing:
		break;

	default:
		break;
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

	GetWorld()->SweepMultiByChannel( // Start부터 끝까지 Sweep으로 충돌 검사
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
		if (!HitActor || !IsValidGrabTarget(HitActor)) continue; //잡을 수 있는 액터 찾기
		
		AActor* OwnerActor = GetOwner();
		if (HitActor == OwnerActor || HitActor->GetOwner() == OwnerActor || OwnerActor->GetOwner() == HitActor)
		{
			continue;
		}
		
		float DistSq = FVector::DistSquared(Hit.ImpactPoint, GetOwner()->GetActorLocation()); // 액터 위치와의 거리
		if (DistSq < ClosestDistanceSq) // 가장 가까운 액터 찾기
		{
			ClosestDistanceSq = DistSq;
			ClosestActor = HitActor;

			TArray<FName> SocketNames = ClosestActor->GetRootComponent()->GetAllSocketNames();

			bool bFoundSocket = false;

			for (const FName& SocketName : SocketNames)
			{
				if (SocketName == RequiredSocketName)
				{
					USceneComponent* RootComp = ClosestActor->GetRootComponent();
					if (RootComp && RootComp->DoesSocketExist(SocketName))
					{
						CurrentTargetLocation = RootComp->GetSocketLocation(SocketName);
						bFoundSocket = true;
					}
					break;
				}
			}

			if (!bFoundSocket)
			{
				CurrentTargetLocation = Hit.ImpactPoint;
			}
		}
	}

	CurrentTarget = ClosestActor; // 가장 가까운 액터를 현재 타겟으로 설정


#if WITH_EDITOR // 디버그용. 구로 Sweep 하면 캡슐이니 캡슐로 표시함.
	FVector SweepCenter = (Start + End) * 0.5f;
	FVector CapsuleDirection = End - Start;
	float HalfHeight = CapsuleDirection.Size() * 0.5f;
	FQuat CapsuleRotation = FRotationMatrix::MakeFromZ(CapsuleDirection).ToQuat();

	DrawDebugCapsule(
		GetWorld(),
		SweepCenter,
		HalfHeight,
		DetectionRadius,
		CapsuleRotation,
		FColor::Green,
		false,
		0.1f
	);
	DrawDebugSphere(GetWorld(), CurrentTargetLocation, DetectionRadius, 12, FColor::Red, false, 0.1f);
#endif
}

void UGrabComponent::StartGrab()
{
	SetGrabState(EGrabState::Detecting);
}

void UGrabComponent::StopGrab()
{
	if (CurrentState == EGrabState::Grabbing) return;
	
	CurrentTarget = nullptr;
	SetGrabState(EGrabState::Idle);
}

void UGrabComponent::SetGrabState(EGrabState NewState)
{
	if (CurrentState == NewState) return;

	switch (CurrentState)
	{
	case EGrabState::Grabbing:
			break;
	default:
		break;
	}

	CurrentState = NewState;

	switch (NewState)
	{
	case EGrabState::Idle:
		CurrentTarget = nullptr;
		break;

	case EGrabState::Grabbing:
		break;

	default:
		break;
	}
}

EGrabState UGrabComponent::GetCurrentGrabState()
{
	return CurrentState;
}

AActor* UGrabComponent::GetGrabTargetActor()
{
	return GrabbedTargetInfo.TargetActor ? GrabbedTargetInfo.TargetActor : nullptr;
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

void UGrabComponent::Grabbed(const FGrabTargetInfo& Info)
{
	SetGrabState(EGrabState::Grabbing);
	GrabbedTargetInfo = Info;
	OnGrabbed.Broadcast(GrabbedTargetInfo);
	
	IGrabbable::Execute_OnGrabbed(GetOwner(), Info.TargetActor);
	IGrabbable::Execute_OnGrabbedBy(Info.TargetActor, GetOwner());
}

void UGrabComponent::Released()
{
	SetGrabState(EGrabState::Idle);
	if (GrabbedTargetInfo.TargetActor)
	{
		IGrabbable::Execute_OnGrabReleased(GetOwner(), GrabbedTargetInfo.TargetActor);
		IGrabbable::Execute_OnGrabReleasedBy(GrabbedTargetInfo.TargetActor, GetOwner());
	}
	
	OnGrabRelease.Broadcast(GrabbedTargetInfo);
	
	GrabbedTargetInfo = {};
	if (GrabHandler)
	{
		GrabHandler->ReleaseGrab();
	}
}

void UGrabComponent::SetGrabHandler(TObjectPtr<UGrabHandler> InGrabHandler)
{
	GrabHandler = InGrabHandler;
}
