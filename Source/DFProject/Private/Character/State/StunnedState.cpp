// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/State/StunnedState.h"

#include "Character/DFCharacter.h"
#include "Components/CapsuleComponent.h"

void UStunnedState::Tick(ADFCharacter* Character, float DeltaTime)
{
	FVector MeshLocation = Character->GetMesh()->GetComponentLocation() - Character->MeshOffset.GetLocation();
	FVector NewCapsuleLocation = FVector(MeshLocation.X, MeshLocation.Y, MeshLocation.Z);
	Character->SetActorLocation(NewCapsuleLocation);

	FName ReferenceBone = TEXT("Hips"); // 또는 pelvis, root 등
	FTransform BoneTransform = Character->GetMesh()->GetSocketTransform(ReferenceBone, RTS_World);
	FRotator TargetRotation = BoneTransform.GetRotation().Rotator() - Character->MeshOffset.Rotator();
	TargetRotation.Pitch = 0.0f;
	TargetRotation.Roll = 0.0f;
	Character->SetActorRotation(TargetRotation);
}

void UStunnedState::Enter(ADFCharacter* Character)
{
	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	auto SMesh = Character->GetMesh();
	SMesh->bPauseAnims = true;
	SMesh->SetSimulatePhysics(true); // 메시에 피직스 적용 (모두 다)
	Character->PhysicalAnimComp->SetStrengthMultiplyer(0.0f); // 완전한 래그돌처럼 보이기 위해 래그돌 비율을 최대로
}

void UStunnedState::Exit(ADFCharacter* Character)
{
	Super::Exit(Character);
}
