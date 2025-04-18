// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Grab/GrabComponent.h"

#include "Ability/Grab/BodyPartGrabHandler.h"
#include "Ability/Grab/Grabbable.h"
#include "Ability/Grab/GrabHandler.h"
#include "Character/DFCharacter.h"
#include "Character/BodyPart/BodyPart.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values for this component's properties
UGrabComponent::UGrabComponent()
{
	PrimaryComponentTick.bCanEverTick = true;     // Tick 가능하게 설정
	PrimaryComponentTick.bStartWithTickEnabled = true; // 시작 시 Tick 활성화

	bAutoActivate = true; // 자동 활성화
	
	GrabConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("GrabConstraint"));
	GrabConstraint->ConstraintInstance.SetDisableCollision(true);
	GrabConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0.f);
	GrabConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0.f);
	GrabConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0.f);

	GrabConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
	GrabConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
	GrabConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.f);

	GrabConstraint->ConstraintInstance.ProfileInstance.AngularDrive.AngularDriveMode = EAngularDriveMode::TwistAndSwing;
	GrabConstraint->ConstraintInstance.ProfileInstance.AngularDrive.SwingDrive.Stiffness = 50000.f;
	GrabConstraint->ConstraintInstance.ProfileInstance.AngularDrive.SwingDrive.Damping = 1000.f;
	GrabConstraint->ConstraintInstance.ProfileInstance.AngularDrive.TwistDrive.Stiffness = 50000.f;
	GrabConstraint->ConstraintInstance.ProfileInstance.AngularDrive.TwistDrive.Damping = 1000.f;
}

// Called when the game starts
void UGrabComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);
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
		}
		break;

	case EGrabState::Grabbing:
		break;

	default:
		break;
	}
}

bool UGrabComponent::HasValidHandler() const
{
	return GrabHandler != nullptr;
}

UBodyPartGrabHandler* UGrabComponent::GetGrabHandler() const
{
	return GrabHandler;
}

void UGrabComponent::CreateHandler(ABodyPart* BodyPart)
{
	if (BodyPart == nullptr)
	{
		UE_LOG(LogDamaged, Warning, TEXT("BodyPart is nullptr!"));
		return;
	}
	GrabHandler = NewObject<UBodyPartGrabHandler>(this);

	if (GrabHandler == nullptr)
	{
		UE_LOG(LogDamaged, Warning, TEXT("GrabHandler creation failed!"));
		return;
	}
	GrabHandler->Initialize(BodyPart);
	GrabHandler->SetOwningGrabComponent(this);
	GrabHandler->SetGrabConstraint(GrabConstraint);
	
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

		if (DistSq >= ClosestDistanceSq)
		{
			continue;
		}

		USceneComponent* RootComp = IGrabbable::Execute_GetRoot(HitActor);
		if (!RootComp)
		{
			continue;
		}

		const TArray<FName> SocketNames = IGrabbable::Execute_GetGrabSocketNames(HitActor);
		const bool bHasSockets = SocketNames.Num() > 0;

		if (!bHasSockets)
		{
			// 소켓이 전혀 없다면: 액터 유지 + 임팩트 포인트
			ClosestDistanceSq = DistSq;
			ClosestActor = HitActor;
			CurrentTargetLocation = Hit.ImpactPoint;
			continue;
		}

		// 소켓이 있다면 RequiredSocketName을 찾아본다
		if (RootComp->DoesSocketExist(RequiredSocketName))
		{
			ClosestDistanceSq = DistSq;
			ClosestActor = HitActor;
			CurrentTargetLocation = RootComp->GetSocketLocation(RequiredSocketName);
		}
		else
		{
			// 소켓은 있지만 이름이 다르면 무시
			ClosestActor = nullptr;
		}
	}

	CurrentTarget = ClosestActor; // 가장 가까운 액터를 현재 타겟으로 설정

     // 디버그용. 구로 Sweep 하면 캡슐이니 캡슐로 표시함.
	//FVector SweepCenter = (Start + End) * 0.5f;
	//FVector CapsuleDirection = End - Start;
	//float HalfHeight = CapsuleDirection.Size() * 0.5f;
	//FQuat CapsuleRotation = FRotationMatrix::MakeFromZ(CapsuleDirection).ToQuat();
//
	//DrawDebugCapsule(
	//	GetWorld(),
	//	SweepCenter,
	//	HalfHeight,
	//	DetectionRadius,
	//	CapsuleRotation,
	//	FColor::Green,
	//	false,
	//	0.1f
	//);
	//DrawDebugSphere(GetWorld(), CurrentTargetLocation, DetectionRadius, 12, FColor::Red, false, 0.1f);
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
	return GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * FrontOffset; // 오프셋
}

FVector UGrabComponent::ComputeDetectionEnd() const
{
	if (!GetOwner()) return FVector::ZeroVector;
	return ComputeDetectionStart() + GetOwner()->GetActorForwardVector() * DetectionDistance;
}

void UGrabComponent::Grabbed(const FGrabTargetInfo& Info)
{
	if (Info.TargetActor == nullptr) return;
	
	SetGrabState(EGrabState::Grabbing);
	GrabbedTargetInfo = Info;
	OnGrabbed.Broadcast(GrabbedTargetInfo);

	if (GetOwner()->HasAuthority()) Multicast_Grabbed(Info);
	
	IGrabbable::Execute_OnGrabbed(GetOwner(), Info.TargetActor);
	IGrabbable::Execute_OnGrabbedBy(Info.TargetActor, GetOwner(), GrabConstraint);
}

void UGrabComponent::Multicast_Grabbed_Implementation(const FGrabTargetInfo& Info)
{
	if (GrabHandler)
	{
		GrabHandler->ExecuteGrab(Info);
	}
}

void UGrabComponent::InitializeGrabHandler_Implementation(EBodyPartType Type)
{
	if (!GetOwner()) return;
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
	
	if (GetOwner()->HasAuthority()) Multicast_Released();
}

void UGrabComponent::Multicast_Released_Implementation()
{
	GrabbedTargetInfo = {};
	if (GrabHandler)
	{
		GrabHandler->ReleaseGrab();
	}
}
