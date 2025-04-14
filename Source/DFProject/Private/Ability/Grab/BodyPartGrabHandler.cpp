// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Grab/BodyPartGrabHandler.h"

#include "Character/DFCharacter.h"
#include "Character/BodyPart/BodyPart.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"


UBodyPartGrabHandler::UBodyPartGrabHandler()
{
	GrabConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>("GrabConstraint");
	GrabConstraint->ConstraintInstance.SetDisableCollision(true);
	GrabConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0.f);
	GrabConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0.f);
	GrabConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0.f);

	GrabConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 45.f);
	GrabConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 45.f);
	GrabConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 45.f);
}

void UBodyPartGrabHandler::BeginDestroy()
{
	Super::BeginDestroy();

	if (GrabConstraint)
	{
		GrabConstraint->BreakConstraint();
	}

	//GrabConstraint = nullptr;
	//Root = nullptr;
	//CurrentGrabTarget = nullptr;
}

void UBodyPartGrabHandler::Initialize(ABodyPart* BodyPart)
{
	Root = BodyPart->GetRootComponent();
	GrabCollider = BodyPart->GetBodyCollider();
	GrabCollider->OnComponentBeginOverlap.AddDynamic(this, &UBodyPartGrabHandler::OnGrabColliderBeginOverlap);

	if (GrabConstraint && Root.IsValid())
	{
		GrabConstraint->AttachToComponent(Root.Get(), FAttachmentTransformRules::KeepRelativeTransform);
	}
	
}

void UBodyPartGrabHandler::MoveToTarget(const FVector& TargetLocation)
{
	if (!Root.IsValid()) return;
	FVector Current = Root->GetComponentLocation();

	UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(Root);
	
	if (RootPrimitive && RootPrimitive->IsSimulatingPhysics())
	{
		FVector ForceDirection = (TargetLocation - Current).GetSafeNormal();
		RootPrimitive->AddForce(ForceDirection * MoveForce);
	}
}

void UBodyPartGrabHandler::ExecuteGrab(const FGrabTargetInfo& TargetInfo)
{
	if (!TargetInfo.TargetActor || !GrabConstraint || !GrabCollider.IsValid()) return;
	if (Cast<ADFCharacter>(TargetInfo.TargetActor)) return; // 하드코딩함 바꿔야함.

	Super::ExecuteGrab(TargetInfo);

	GrabCollider->IgnoreActorWhenMoving(TargetInfo.TargetActor, true);

	
	UPrimitiveComponent* TargetRoot = IGrabbable::Execute_GetRoot(TargetInfo.TargetActor);

	GrabConstraint->SetConstrainedComponents(
		Cast<UPrimitiveComponent>(GrabCollider),
		NAME_None,
		Cast<UPrimitiveComponent>(TargetInfo.TargetActor->GetRootComponent()),
		NAME_None
	);

	if (!GrabConstraint->ConstraintInstance.IsValidConstraintInstance())
	{
		UE_LOG(LogTemp, Warning, TEXT("[GrabHandler] ConstraintInstance is INVALID after setting!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[GrabHandler] Constraint successfully initialized."));
	}
	
	CurrentGrabTarget = TargetInfo.TargetActor;
}

void UBodyPartGrabHandler::ReleaseGrab()
{
	UE_LOG(LogTemp, Warning, TEXT("[GrabHandler] ReleaseGrab called"));
	Super::ReleaseGrab();
	if (!GrabConstraint) return;
	UE_LOG(LogTemp, Warning, TEXT("[GrabHandler] Breaking constraint now"));
	GrabConstraint->BreakConstraint();
	CurrentGrabTarget = nullptr;
}
