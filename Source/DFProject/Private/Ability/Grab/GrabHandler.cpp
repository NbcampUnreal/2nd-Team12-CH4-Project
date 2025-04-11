// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Grab/GrabHandler.h"

#include "Ability/Grab/Grabbable.h"
#include "Ability/Grab/GrabComponent.h"

void UGrabHandler::SetOwningGrabComponent(UGrabComponent* GrabComp)
{
	OwningGrabComponent = GrabComp;
}

bool UGrabHandler::IsGrabbable(const AActor* Target)
{
	if (!Target || !Target->GetClass()->ImplementsInterface(UGrabbable::StaticClass()))
	{
		return false;
	}
	
	return true;
}

void UGrabHandler::ExecuteGrab(const FGrabTargetInfo& TargetInfo)
{
	OwningGrabComponent->Grabbed(TargetInfo);
	OnGrabStart.Broadcast(TargetInfo);
}

void UGrabHandler::ReleaseGrab()
{
	OnGrabRelease.Broadcast();
}

void UGrabHandler::OnGrabColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OwningGrabComponent || OwningGrabComponent->GetCurrentGrabState() != EGrabState::Detecting) return;
	if (!IsGrabbable(OtherActor)) return;

	AActor* MyOwner = OwningGrabComponent->GetOwner();
	AActor* OtherOwner = OtherActor ? OtherActor->GetOwner() : nullptr;

	if (OtherActor == MyOwner || OtherOwner == MyOwner)
		return;
	
	FGrabTargetInfo TargetInfo;
	
	
	TargetInfo.TargetActor = OtherActor;
	TargetInfo.TargetComponent = OtherComp;
	TargetInfo.HitLocation = SweepResult.ImpactPoint;
	TargetInfo.HitNormal = SweepResult.ImpactNormal;

	ExecuteGrab(TargetInfo);
}
