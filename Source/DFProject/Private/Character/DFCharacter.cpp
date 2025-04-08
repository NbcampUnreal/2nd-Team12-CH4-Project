// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DFCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/BodyPart/BodyPart.h"
#include "Character/BodyPart/AttachInfoComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

// Sets default values
ADFCharacter::ADFCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(SpringArm);

	PhysicalAnimComp = CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("PhysicalAnimComp"));

	PhysicalAnimData.bIsLocalSimulation = true;
	PhysicalAnimData.OrientationStrength = 1000.0f;
	PhysicalAnimData.AngularVelocityStrength = 100.0f;
	PhysicalAnimData.PositionStrength = 1000.0f;
	PhysicalAnimData.VelocityStrength = 100.0f;
	PhysicalAnimData.MaxLinearForce = 100.0f;
	PhysicalAnimData.MaxAngularForce = 100.0f;
}

// Called when the game starts or when spawned
void ADFCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->bPauseAnims = true;
	MeshOffset = GetMesh()->GetRelativeTransform();

	ApplyPhysicalAnimationSettings();
}

void ADFCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRecovering)
	{
		PhysicalAnimComp->SetStrengthMultiplyer(RecoverAlpha);

		if (USkeletalMeshComponent* SMesh = GetMesh())
		{
			// 현재 회전
			FQuat CurrentQuat = SMesh->GetComponentQuat();

			// 목표 회전 계산 (Yaw은 Initial 유지, Pitch & Roll은 MeshOffset 기준)
			FRotator InitialRotator = InitialRecoveryRotation.Rotator();
			FRotator MeshOffsetRotator = MeshOffset.GetRotation().Rotator();

			FRotator TargetRotator;
			TargetRotator.Yaw = InitialRotator.Yaw;
			TargetRotator.Pitch = MeshOffsetRotator.Pitch;
			TargetRotator.Roll = MeshOffsetRotator.Roll;

			FQuat TargetQuat = TargetRotator.Quaternion();

			// 회복 알파 계산 (0에서 1로 서서히 증가)
			RecoverAlpha = FMath::Clamp(RecoverAlpha + DeltaTime * RecoverSpeed, 0.f, 1.f);

			// 부드럽게 회전 보간
			FQuat BlendedQuat = FQuat::Slerp(CurrentQuat, TargetQuat, RecoverAlpha);
			FQuat DeltaQuat = BlendedQuat * CurrentQuat.Inverse();
			DeltaQuat.Normalize();

			FVector Axis;
			float Angle;
			DeltaQuat.ToAxisAndAngle(Axis, Angle);

			if (Angle > KINDA_SMALL_NUMBER)
			{
				FVector Torque = Axis * Angle * 100.0f; //Torque Strength로 변수화 해야함
				SMesh->AddTorqueInRadians(Torque, NAME_None, true);
			}

			// 정렬되었으면 회복 완료
			const float AngleThreshold = 2.0f * (PI / 180.0f); // 2도
			if (Angle < AngleThreshold)
			{
				FinishGetUp();
				bIsRecovering = false;
				RecoverAlpha = 0.f;
			}
		}

		// 부드러운 회복 힘 증폭
		RecoverAlpha = FMath::Min(RecoverAlpha + DeltaTime * RecoverSpeed, 1.0f);
	}
	
	if (!bIsStunned) return;

	FVector MeshLocation = GetMesh()->GetComponentLocation() - MeshOffset.GetLocation();
	FVector NewCapsuleLocation = FVector(MeshLocation.X, MeshLocation.Y, MeshLocation.Z);
	SetActorLocation(NewCapsuleLocation);

	FName ReferenceBone = TEXT("Hips"); // 또는 pelvis, root 등
	FTransform BoneTransform = GetMesh()->GetSocketTransform(ReferenceBone, RTS_World);
	FRotator TargetRotation = BoneTransform.GetRotation().Rotator() - MeshOffset.Rotator();
	TargetRotation.Pitch = 0.0f;
	TargetRotation.Roll = 0.0f;
	SetActorRotation(TargetRotation);

}

// Called to bind functionality to input
void ADFCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	
	if (!EnhancedInputComponent) return;

	EnhancedInputComponent->BindAction(
		MoveAction,
		ETriggerEvent::Triggered,
		this,
		&ADFCharacter::Move
	);
	
	EnhancedInputComponent->BindAction(
		LookAction,
		ETriggerEvent::Triggered,
		this,
		&ADFCharacter::Look
	);
	
	EnhancedInputComponent->BindAction(
		SprintAction,
		ETriggerEvent::Triggered,
	this,
		&ADFCharacter::Sprint
	);
	
	EnhancedInputComponent->BindAction(
		JumpAction,
		ETriggerEvent::Triggered,
		this,
		&ADFCharacter::Jump
	);

	EnhancedInputComponent->BindAction(
	PunchAction,
	ETriggerEvent::Triggered,
	this,
	&ADFCharacter::Punch
	);
	
	EnhancedInputComponent->BindAction(
	DropKickAction,
	ETriggerEvent::Triggered,
	this,
	&ADFCharacter::DropKick
	);

	EnhancedInputComponent->BindAction(
	HeadbuttAction,
	ETriggerEvent::Triggered,
	this,
	&ADFCharacter::Headbutt
	);
}

void ADFCharacter::Move(const FInputActionValue& Value)
{
	if (bIsStunned) return;
	const FVector2D MoveValue = Value.Get<FVector2D>();
	
	// 카메라의 현재 Yaw(좌우) 회전 값을 기준으로 이동 방향 설정
	const FRotator CameraRotation = Camera->GetComponentRotation();
	const FRotator MovementRotation(0.f, CameraRotation.Yaw, 0.0f);

	FVector MovementDirection = FVector::ZeroVector;

	if (MoveValue.X != 0.0f)
	{
		MovementDirection += MovementRotation.RotateVector(FVector::ForwardVector) * MoveValue.X;
	}

	if (MoveValue.Y != 0.0f)
	{
		MovementDirection += MovementRotation.RotateVector(FVector::RightVector) * MoveValue.Y;
	}

	// 이동 방향 적용
	if (!MovementDirection.IsNearlyZero())
	{
		FRotator CurrentRotation = GetActorRotation();
		
		FRotator TargetRotation = MovementDirection.Rotation();

		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 5.0f); // 5.0f는 회전 속도 (더 높이면 빠르게 회전)

		// 클라이언트 예측
		SetActorRotation(NewRotation);
		// CharacterMovementComponent는 Replication 지원해서 AddMovementInput으로
		AddMovementInput(MovementDirection.GetSafeNormal());
		
		if (!HasAuthority()) Server_Move(NewRotation); //클라면 서버에게 요청
		else Multicast_Move(NewRotation);  // 서버(리슨서버 주인)이면 바로 멀티캐스트 
	}
}

void ADFCharacter::Server_Move_Implementation(const FRotator& NewRotation)
{
	Multicast_Move_Implementation(NewRotation);
}

void ADFCharacter::Multicast_Move_Implementation(const FRotator& NewRotation)
{
	SetActorRotation(NewRotation);
}

void ADFCharacter::Sprint(const FInputActionValue& Value)
{
	if (bIsStunned) return;
}

void ADFCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookValue = Value.Get<FVector2D>();

	if (!SpringArm) return;

	FRotator CurrentRotation = SpringArm->GetRelativeRotation();

	// X축(Yaw): 좌우 회전 (캐릭터 회전과 분리)
	if (LookValue.X != 0.0f)
	{
		CurrentRotation.Yaw += LookValue.X;
	}

	// Y축(Pitch): 위아래 회전 (카메라 위아래 움직임)
	if (LookValue.Y != 0.0f)
	{
		//CurrentRotation.Pitch = FMath::Clamp(CurrentRotation.Pitch + LookValue.Y, -80.0f, 80.0f); // 상하 제한
	}

	SpringArm->SetRelativeRotation(CurrentRotation);
}


void ADFCharacter::Grab(const FInputActionValue& Value)
{
	Server_Grab();
}

void ADFCharacter::Server_Grab_Implementation()
{
	if (bIsStunned) return;
}

void ADFCharacter::DropKick(const FInputActionValue& Value)
{
	Server_DropKick();
}

void ADFCharacter::Server_DropKick_Implementation()
{
	if (bIsStunned) return;
	
	if (!BodyParts.Contains(EBodyPartType::LeftFoot) || !BodyParts.Contains(EBodyPartType::RightFoot)) return;
	if (!BodyParts[EBodyPartType::LeftFoot] || !BodyParts[EBodyPartType::RightFoot]) return;

	if (!GetCharacterMovement()->IsFalling()) return;

	BodyParts[EBodyPartType::LeftFoot]->ApplyImpulse();
	BodyParts[EBodyPartType::RightFoot]->ApplyImpulse();
}

void ADFCharacter::Headbutt(const FInputActionValue& Value)
{
	Server_Headbutt();
}

void ADFCharacter::Server_Headbutt_Implementation()
{
	if (bIsStunned) return;
	
	if (!BodyParts.Contains(EBodyPartType::Head) || !BodyParts[EBodyPartType::Head]) return;

	BodyParts[EBodyPartType::Head]->ApplyImpulse();
}


void ADFCharacter::StartJump(const FInputActionValue& Value)
{
	if (bIsStunned) return;
	Jump();
}

void ADFCharacter::SpawnBodyParts()
{
	if (HasAuthority()) //서버에선 바디파츠 생성. 바디파츠는 복제됨
	{
		TArray<UAttachInfoComponent*> AttachInfos;
		GetComponents<UAttachInfoComponent>(AttachInfos);

		for (UAttachInfoComponent* Info : AttachInfos)
		{
			if (!Info || !IsValid(Info->BodyPartClass)) continue;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
		
			ABodyPart* SpawnedPart = GetWorld()->SpawnActor<ABodyPart>(Info->BodyPartClass);
			if (!SpawnedPart) continue;

			SpawnedPart->SetReplicates(true);
			SpawnedPart->Attach(this, Info);

			BodyParts.Add(Info->BodyPartType, SpawnedPart);
		}
	}
	
	GetMesh()->bPauseAnims = false; // 클라는 애니메이션 재생 시작
}

void ADFCharacter::Punch(const FInputActionValue& Value)
{
	Server_Punch();
}

void ADFCharacter::Server_Punch_Implementation()
{
	if (bIsStunned) return;
	if (!BodyParts.Contains(EBodyPartType::LeftFist) || !BodyParts.Contains(EBodyPartType::RightFist)) return;
	if (!BodyParts[EBodyPartType::LeftFist] || !BodyParts[EBodyPartType::RightFist]) return;
	
	
	if (bLeft) BodyParts[EBodyPartType::LeftFist]->ApplyImpulse();
	else BodyParts[EBodyPartType::RightFist]->ApplyImpulse();

	bLeft = !bLeft;	
}

void ADFCharacter::ApplyPhysicalAnimationSettings()
{
	if (!PhysicalAnimComp || !GetMesh()) return;
	
	PhysicalAnimComp->SetSkeletalMeshComponent(GetMesh());
	PhysicalAnimComp->ApplyPhysicalAnimationSettingsBelow(PhysicalAnimStartBone, PhysicalAnimData, false);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PhysicalAnimStartBone, true, false);
}

void ADFCharacter::Stun()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	auto SMesh = GetMesh();
	SMesh->bPauseAnims = true;
	SMesh->SetSimulatePhysics(true); // 메시에 피직스 적용
	PhysicalAnimComp->SetStrengthMultiplyer(0.0f); // 완전한 래그돌처럼 보이기 위해 래그돌 비율을 최대로
	bIsStunned = true;
}

void ADFCharacter::RecoverFromStun()
{
	PhysicalAnimComp->SetStrengthMultiplyer(0.0f);
	bIsRecovering = true;
	RecoverAlpha = 0.0f;
	
	InitialRecoveryRotation = GetMesh()->GetComponentQuat();
}

void ADFCharacter::FinishGetUp()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	auto SMesh = GetMesh();
	SMesh->SetSimulatePhysics(false);
	
	PhysicalAnimComp->SetStrengthMultiplyer(0.5f);
	SMesh->SetRelativeTransform(MeshOffset);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PhysicalAnimStartBone, true, false);
	
	SMesh->bPauseAnims = false;
	bIsStunned = false;

	//FVector Up = GetMesh()->GetUpVector();
	//
	//if (FVector::DotProduct(Up, FVector::UpVector) < 0.f)
	//{
	//	PlayAnimMontage(GetUpFrontMontage);
	//}
	//else
	//{
	//	PlayAnimMontage(GetUpBackMontage);		
	//}
	
}
