// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BodyPart/BodyPart.h"

#include "Character/BodyPart/AttachInfoComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values
ABodyPart::ABodyPart()
{
	PrimaryActorTick.bCanEverTick = true;
	BodyCollider = CreateDefaultSubobject<USphereComponent>(TEXT("BodyCollider"));
	SetRootComponent(BodyCollider);
	BodyCollider->SetCollisionProfileName(TEXT("BodyPart"));

	BoneConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("HandConstraint"));

	BoneConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 45.f);
	BoneConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 45.f);
	BoneConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 45.f);

	BoneConstraint->SetDisableCollision(true);

	SetReplicates(true);
	SetReplicateMovement(false);
	BodyCollider->SetIsReplicated(true);
	BoneConstraint->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void ABodyPart::BeginPlay()
{
	Super::BeginPlay();
	
	BodyCollider->SetHiddenInGame(false);
	BodyCollider->SetVisibility(true);	
}

void ABodyPart::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (BoneConstraint) BoneConstraint->BreakConstraint();

	SetOwner(nullptr);
	OwningCharacter = nullptr;
}

void ABodyPart::BeginDestroy()
{
	Super::BeginDestroy();
	
	if (BoneConstraint) BoneConstraint->BreakConstraint();
	if (GrabberConstraint.Get()) GrabberConstraint->BreakConstraint(); 
}

void ABodyPart::Attach_Implementation(ACharacter* TargetCharacter, const UAttachInfoComponent* AttachInfo)
{
	if (!TargetCharacter || !AttachInfo || !TargetCharacter->GetMesh()) return;
	
	SetOwner(TargetCharacter);
	OwningCharacter = TargetCharacter; // 그냥 Owner로 할까 고민
	
	USkeletalMeshComponent* Mesh = TargetCharacter->GetMesh();
	
	// AttachInfo 정보로 부착 및 위치, 회전, 크기 설정.
	BoneToAttach = AttachInfo->TargetBoneName;

	// 본에 상대적으로 부착
	BodyCollider->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, BoneToAttach);

	// 본 위치에서 오프셋 적용
	BodyCollider->SetRelativeTransform(GetOffsetTransform(TargetCharacter, AttachInfo));
	
	// Constraint로 연결하기 전 물리속성 설정 (생성자에서 하면 원하는 중력 때문에 아래로 떨어진 뒤 연결됨)
	BodyCollider->SetSimulatePhysics(true);
	BodyCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BodyCollider->SetCollisionObjectType(ECC_PhysicsBody);
	BodyCollider->SetMassOverrideInKg(NAME_None, 5.f, true);

	// BodyCollider와 메시를 연결
	BoneConstraint->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, BoneToAttach);
	BoneConstraint->SetConstrainedComponents(BodyCollider, NAME_None, Mesh, BoneToAttach);
}

FTransform ABodyPart::GetOffsetTransform(const ACharacter* TargetCharacter, const UAttachInfoComponent* AttachInfo)
{
	if (!TargetCharacter || !AttachInfo) return FTransform::Identity;

	// 메시와 타겟 본 이름 확보
	USkeletalMeshComponent* Mesh = TargetCharacter->GetMesh();
	const FName& BoneName = AttachInfo->TargetBoneName;

	// 해당 본의 월드 트랜스폼 얻기
	const FTransform BoneWorldTransform = Mesh->GetSocketTransform(BoneName, RTS_World);

	// AttachInfo의 월드 트랜스폼
	const FTransform AttachInfoTransform = AttachInfo->GetComponentTransform();

	// AttachInfo가 Bone 기준으로 얼마나 떨어져 있는가 = Offset
	return AttachInfoTransform.GetRelativeTransform(BoneWorldTransform);
}

void ABodyPart::OnRep_TargetTransform()
{
	TargetLocation = ReplicatedTransform.GetLocation();
	TargetLocation = ReplicatedTransform.GetLocation();
}

void ABodyPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (HasAuthority())
	//{
	//	TimeSinceLastUpdate += DeltaTime;
	//	if (TimeSinceLastUpdate >= UpdateInterval) // 예: 0.1초 간격
	//	{
	//		ReplicatedTransform = GetActorTransform();
	//		TimeSinceLastUpdate = 0.0f;
	//	}
	//}
	//else
	//{
	//	FVector NewLoc = FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, InterpSpeed);
	//	FQuat NewRot = FMath::QInterpTo(GetActorQuat(), TargetRotation, DeltaTime, InterpSpeed);
//
	//	SetActorLocationAndRotation(NewLoc, NewRot, false, nullptr, ETeleportType::TeleportPhysics);
	//}
}

void ABodyPart::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABodyPart, ReplicatedTransform);
}

TObjectPtr<USphereComponent> ABodyPart::GetBodyCollider()
{
	return BodyCollider;
}

TObjectPtr<ACharacter> ABodyPart::GetOwningCharacter()
{
	return OwningCharacter;
}

AActor* ABodyPart::GetActualTarget_Implementation()
{
	return Owner;
}

FVector ABodyPart::GetResistanceForce_Implementation(AActor* PullingActor)
{
	if (Owner && Owner->Implements<UGrabbable>())
	{
		return IGrabbable::Execute_GetResistanceForce(Owner, PullingActor);
	}
	return FVector::ZeroVector;
}

void ABodyPart::OnGrabbed_Implementation(AActor* Grabber)
{
	if (Owner && Owner->Implements<UGrabbable>())
	{
		return IGrabbable::Execute_OnGrabbed(Owner, Grabber);
	}
}

void ABodyPart::OnGrabbedBy_Implementation(AActor* Grabber, UPhysicsConstraintComponent* InGrabberConstraint)
{
	if (Owner && Owner->Implements<UGrabbable>())
	{
		GrabberConstraint = InGrabberConstraint;
		return IGrabbable::Execute_OnGrabbedBy(Owner, Grabber, InGrabberConstraint);
	}
}

void ABodyPart::OnGrabReleased_Implementation(AActor* Grabber)
{
	if (Owner && Owner->Implements<UGrabbable>())
	{
		return IGrabbable::Execute_OnGrabReleased(Owner, Grabber);
	}
}

void ABodyPart::OnGrabReleasedBy_Implementation(AActor* Grabber)
{
	if (Owner && Owner->Implements<UGrabbable>())
	{
		return IGrabbable::Execute_OnGrabReleasedBy(Owner, Grabber);
	}
}

UPrimitiveComponent* ABodyPart::GetRoot_Implementation()
{
	return BodyCollider;
}

void ABodyPart::Multicast_AddImpulse_Implementation(const FVector& Impulse)
{
	BodyCollider->AddImpulse(Impulse, NAME_None, true);
}

void ABodyPart::Multicast_AddForce_Implementation(const FVector& Force)
{
	BodyCollider->AddForce(Force, NAME_None, true);
}
