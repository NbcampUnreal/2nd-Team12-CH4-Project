// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DFCharacter.h"

#include "EnhancedInputComponent.h"
#include "Ability/HeadbuttAbilityStrategy.h"
#include "Ability/PunchAbilityStrategy.h"
#include "Ability/Grab/GrabComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/BodyPart/AFist.h"
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

			// Angle 로그 찍기 (실제 Target과의 차이)
			const float AngleThreshold = 2.0f * (PI / 180.0f);
			UE_LOG(LogTemp, Log, TEXT("[Recovery] Current Angle: %.2f degrees / Threshold: %.2f degrees"), 
				FMath::RadiansToDegrees(Angle), 
				FMath::RadiansToDegrees(AngleThreshold));

			if (Angle < AngleThreshold || RecoverAlpha > 0.95f)
			{
				FinishGetUp();
				bIsRecovering = false;
				RecoverAlpha = 0.f;
			}
		}
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

	if (!BodyParts.Contains(EBodyPartType::LeftFist) || !BodyParts.Contains(EBodyPartType::RightFist)) return;

	if (AFist* LeftFist = Cast<AFist>(BodyParts[EBodyPartType::LeftFist]))
	{
		if (UGrabComponent* GrabComp = LeftFist->FindComponentByClass<UGrabComponent>())
		{
			GrabComp->StartGrab();
		}
	}

	if (AFist* RightFist = Cast<AFist>(BodyParts[EBodyPartType::RightFist]))
	{
		if (UGrabComponent* GrabComp = RightFist->FindComponentByClass<UGrabComponent>())
		{
			GrabComp->StartGrab();
		}
	}
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

	UAbilityStrategy* PunchStrategy = NewObject<UHeadbuttAbilityStrategy>(this); // this = Outer

	BodyParts[EBodyPartType::Head]->SetAttackStrategy(PunchStrategy);
	BodyParts[EBodyPartType::Head]->PerformAttack();
}


void ADFCharacter::StartJump(const FInputActionValue& Value)
{
	if (bIsStunned)
	{
		RecoverHandleInput(); // 연타 처리 함수
		return;
	}
	
	Super::Jump();
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

	ABodyPart* Fist = bLeft ? BodyParts[EBodyPartType::LeftFist] : BodyParts[EBodyPartType::RightFist];
	if (!Fist) return;
	UAbilityStrategy* PunchStrategy = NewObject<UPunchAbilityStrategy>(this); // this = Outer

	Fist->SetAttackStrategy(PunchStrategy);
	Fist->PerformAttack();

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
	SMesh->SetSimulatePhysics(true); // 메시에 피직스 적용 (모두 다)
	PhysicalAnimComp->SetStrengthMultiplyer(0.0f); // 완전한 래그돌처럼 보이기 위해 래그돌 비율을 최대로
	bIsStunned = true;

	GetWorldTimerManager().SetTimer(RecoverTimer, this, &ADFCharacter::RecoverStart, 5.f, false);
}

void ADFCharacter::RecoverStart()
{
	if (bIsRecovering) return;
	
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

	HP = MaxHP;
	SMesh->bPauseAnims = false;
	bIsStunned = false;
}

void ADFCharacter::RecoverHandleInput()
{
	if (!bIsStunned) return;

	RecoverInputCount++;

	if (RecoverInputCount >= RecoverInputGoal)
	{
		GetWorldTimerManager().ClearTimer(RecoverTimer);
		RecoverStart();
	}
}

float ADFCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	float DamageApplied = FMath::Min(HP, DamageAmount);
	HP -= DamageApplied;

	UE_LOG(LogTemp, Log, TEXT("[%s] 데미지 받음: %.2f, 남은 HP: %.2f (가해자: %s)"),
		*GetName(), DamageApplied, HP - DamageApplied,
		DamageCauser ? *DamageCauser->GetName() : TEXT("알 수 없음"));
	
	if (bIsStunned) return DamageApplied;

	HP -= DamageApplied;

	if (HP <= 0.f)
	{
		HP = 0.f;
		Stun();
	}

	return DamageApplied;
}
