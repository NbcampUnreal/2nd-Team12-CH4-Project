// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/State/RecoverState.h"

#include "Character/DFCharacter.h"

void URecoverState::Tick(ADFCharacter* Character, float DeltaTime)
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
	
	Character->PhysicalAnimComp->SetStrengthMultiplyer(RecoverAlpha);

	if (USkeletalMeshComponent* SMesh = Character->GetMesh())
	{
		// 현재 회전
		FQuat CurrentQuat = SMesh->GetComponentQuat();

		// 목표 회전 계산 (Yaw은 Initial 유지, Pitch & Roll은 MeshOffset 기준)
		FRotator InitialRotator = InitialRecoveryRotation.Rotator();
		FRotator MeshOffsetRotator = Character->MeshOffset.GetRotation().Rotator();

		FRotator TargetRotator;
		TargetRotator.Yaw = InitialRotator.Yaw;
		TargetRotator.Pitch = 0;
		TargetRotator.Roll = 0;

		FQuat TargetQuat = TargetRotator.Quaternion();

		// 회복 알파 계산 (0에서 1로 서서히 증가)
		RecoverAlpha = FMath::Clamp(RecoverAlpha + DeltaTime * RecoverSpeed, 0.f, 1.f);

		// 목표 회전과 현재 회전 차이 계산 (이게 실제 회복 체크 기준)
		FQuat DeltaQuat = TargetQuat * CurrentQuat.Inverse();
		DeltaQuat.Normalize();

		FVector Axis;
		float Angle;
		DeltaQuat.ToAxisAndAngle(Axis, Angle);

		// 회복 알파 계산 (0에서 1로 서서히 증가)
		RecoverAlpha = FMath::Clamp(RecoverAlpha + DeltaTime * RecoverSpeed, 0.f, 1.f);

		// 부드럽게 회전 보간해서 Torque 방향 설정
		FQuat BlendedQuat = FQuat::Slerp(CurrentQuat, TargetQuat, RecoverAlpha);
		FQuat TorqueQuat = BlendedQuat * CurrentQuat.Inverse();
		TorqueQuat.Normalize();

		FVector TorqueAxis;
		float TorqueAngle;
		TorqueQuat.ToAxisAndAngle(TorqueAxis, TorqueAngle);

		if (TorqueAngle > KINDA_SMALL_NUMBER)
		{
			FVector Torque = TorqueAxis * TorqueAngle * 5000.0f; // Strength 조절 가능
			SMesh->AddTorqueInRadians(Torque, NAME_None, true);
		}

		const float AngleThreshold = 2.0f * (PI / 180.0f);

		//UE_LOG(LogTemp, Log, TEXT("[Recovery] Current Angle: %.2f degrees / Threshold: %.2f degrees"), 
		//	FMath::RadiansToDegrees(Angle), 
		//	FMath::RadiansToDegrees(AngleThreshold));

		if (Angle < AngleThreshold || RecoverAlpha > 0.95f)
		{
			Character->FinishGetUp();
		}
	}
}

void URecoverState::Enter(ADFCharacter* Character)
{
	RecoverAlpha = 0.f;
	RecoverSpeed = 0.8f;
	Character->PhysicalAnimComp->SetStrengthMultiplyer(0.0f);
	InitialRecoveryRotation = Character->GetMesh()->GetComponentQuat();
}

void URecoverState::Exit(ADFCharacter* Character)
{
}
