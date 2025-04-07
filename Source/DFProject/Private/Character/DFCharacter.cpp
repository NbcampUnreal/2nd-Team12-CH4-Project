// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DFCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/BodyPart/BodyPart.h"
#include "Character/BodyPart/AttachInfoComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

// Sets default values
ADFCharacter::ADFCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
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

	ApplyPhysicalAnimationSettings();

	SpawnBodyParts();
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
}

void ADFCharacter::DropKick(const FInputActionValue& Value)
{
	if (!BodyParts[EBodyPartType::LeftFoot] || !BodyParts[EBodyPartType::RightFoot]) return;
	if (!GetCharacterMovement()->IsFalling()) return;

	BodyParts[EBodyPartType::LeftFoot]->ApplyImpulse();
	BodyParts[EBodyPartType::RightFoot]->ApplyImpulse();
}

void ADFCharacter::Headbutt(const FInputActionValue& Value)
{
	if (!BodyParts[EBodyPartType::Head]) return;

	BodyParts[EBodyPartType::Head]->ApplyImpulse();
}


void ADFCharacter::StartJump(const FInputActionValue& Value)
{
	Jump();
}

void ADFCharacter::SpawnBodyParts()
{
	TArray<UAttachInfoComponent*> AttachInfos;
	GetComponents<UAttachInfoComponent>(AttachInfos);

	for (UAttachInfoComponent* Info : AttachInfos)
	{
		if (!Info || !IsValid(Info->BodyPartClass)) continue;

		ABodyPart* SpawnedPart = GetWorld()->SpawnActor<ABodyPart>(Info->BodyPartClass);
		if (!SpawnedPart) continue;

		SpawnedPart->Attach(this, Info);

		BodyParts.Add(Info->BodyPartType, SpawnedPart);
	}
}

void ADFCharacter::Punch(const FInputActionValue& Value)
{
	if (Left) BodyParts[EBodyPartType::LeftFist]->ApplyImpulse();
	else BodyParts[EBodyPartType::RightFist]->ApplyImpulse();

	Left = !Left;
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
	auto SMesh = GetMesh();
	SMesh->SetSimulatePhysics(true);
	SMesh->SetCollisionProfileName("Ragdoll");
	PhysicalAnimComp->SetSkeletalMeshComponent(nullptr);
}
