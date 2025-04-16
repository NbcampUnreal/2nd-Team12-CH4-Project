// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DFCharacter.h"

#include "EnhancedInputComponent.h"
#include "Ability/Grab/BodyPartGrabHandler.h"
#include "Ability/Grab/GrabComponent.h"
#include "Ability/Strategy/AbilityStrategyManager.h"
#include "Camera/CameraComponent.h"
#include "Character/MovementModifierComponent.h"
#include "Character/BodyPart/BodyPart.h"
#include "Character/BodyPart/AttachInfoComponent.h"
#include "Character/State/CharacterStateManager.h"
#include "Character/State/GrabbedState.h"
#include "Character/State/IdleState.h"
#include "Character/State/RecoverState.h"
#include "Character/State/StunnedState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "DirGravity/Public/GravityMovementComponent.h"

DEFINE_LOG_CATEGORY(LogDamaged);
DEFINE_LOG_CATEGORY(LogInitialize);

ADFCharacter::ADFCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UGravityMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	
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

	LeftGrabComp = CreateDefaultSubobject<UGrabComponent>(TEXT("LeftGrab"));
	
	RightGrabComp = CreateDefaultSubobject<UGrabComponent>(TEXT("RightGrab"));

	MovementModifier = CreateDefaultSubobject<UMovementModifierComponent>(TEXT("MovementModifier"));

	AbilityManager = CreateDefaultSubobject<UAbilityStrategyManager>(TEXT("AbilityManager"));
	
	StateManager = CreateDefaultSubobject<UCharacterStateManager>(TEXT("StateManager"));

	SetAllBonesMass(5.0f);
}

void ADFCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Current MovementComponent: %s"), *GetCharacterMovement()->GetClass()->GetName());
	//UpdateSpringArmOrientation();
	MeshOffset = GetMesh()->GetRelativeTransform();

	GetMesh()->bPauseAnims = true;
}

void ADFCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

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
		ETriggerEvent::Started,
	this,
		&ADFCharacter::StartSprint
	);

	EnhancedInputComponent->BindAction(
		SprintAction,
		ETriggerEvent::Canceled,
	this,
		&ADFCharacter::StopSprint
	);

	EnhancedInputComponent->BindAction(
	SprintAction,
		ETriggerEvent::Completed,
	this,
		&ADFCharacter::StopSprint
	);
	
	EnhancedInputComponent->BindAction(
		JumpAction,
		ETriggerEvent::Triggered,
		this,
		&ADFCharacter::Jump
	);
	
	EnhancedInputComponent->BindAction(
		GrabAction,
		ETriggerEvent::Started, // 누르는 순간
		this,
		&ADFCharacter::StartGrab
	);

	EnhancedInputComponent->BindAction(
	GrabAction,
	ETriggerEvent::Canceled, // 떼는 순간
	this,
	&ADFCharacter::StopGrab
	);

	EnhancedInputComponent->BindAction(
	GrabAction,
	ETriggerEvent::Completed, // 떼는 순간
	this,
	&ADFCharacter::StopGrab
	);
	
	EnhancedInputComponent->BindAction(
	PunchAction,
	ETriggerEvent::Triggered,
	this,
	&ADFCharacter::BasicAttack
	);

	EnhancedInputComponent->BindAction(
	HeadbuttAction,
	ETriggerEvent::Triggered,
	this,
	&ADFCharacter::Headbutt
	);

	EnhancedInputComponent->BindAction(
	ReleaseGrabAction,
	ETriggerEvent::Triggered,
	this,
	&ADFCharacter::ReleaseGrab
	);
}

bool ADFCharacter::EventOnDestroy()
{
	if (RightGrabComp) RightGrabComp->Released();
	if (LeftGrabComp) LeftGrabComp->Released();
	
	return MovementModifier->UnregisterAll();
}

UGravityMovementComponent* ADFCharacter::GetGravityMovementComponent()
{
	return Cast<UGravityMovementComponent>(GetMovementComponent());
}

void ADFCharacter::ReadyToPlay()
{
	RegisterAbilities();
	SpawnBodyParts();
	Multicast_ReadyToPlay();
}

void ADFCharacter::Multicast_ReadyToPlay_Implementation()
{
	ApplyPhysicalAnimationSettings();
	GetMesh()->bPauseAnims = false;
	StateManager->SetState(NewObject<UIdleState>(this));
}

void ADFCharacter::SpawnBodyParts()
{
	if (!HasAuthority()) return; //서버에선 바디파츠 생성. 바디파츠는 복제됨
	
	TArray<UAttachInfoComponent*> AttachInfos;
	GetComponents<UAttachInfoComponent>(AttachInfos);

	for (UAttachInfoComponent* Info : AttachInfos)
	{
		if (!Info || !IsValid(Info->BodyPartClass) || !Info->bAutoSpawnBeginPlay) continue;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
	
		ABodyPart* SpawnedPart = GetWorld()->SpawnActor<ABodyPart>(Info->BodyPartClass);
		if (!SpawnedPart) continue;

		SpawnedPart->SetReplicates(true);
		SpawnedPart->Attach(this, Info);

		BodyParts.Add(Info->BodyPartType, SpawnedPart);
	}

	if (LeftGrabComp && BodyParts.Contains(EBodyPartType::LeftFist))
	{
		UBodyPartGrabHandler* LeftHandler = NewObject<UBodyPartGrabHandler>();
		LeftHandler->SetOwningGrabComponent(LeftGrabComp);
		LeftHandler->Initialize(BodyParts[EBodyPartType::LeftFist]);
		LeftGrabComp->SetGrabHandler(LeftHandler);
	}
	else
	{
		UE_LOG(LogInitialize, Warning, TEXT("Grab Components not initialized."));
	}

	// 오른손 핸들러 생성 및 연결
	if (RightGrabComp && BodyParts.Contains(EBodyPartType::RightFist))
	{
		UBodyPartGrabHandler* RightHandler = NewObject<UBodyPartGrabHandler>();
		RightHandler->SetOwningGrabComponent(RightGrabComp);
		RightHandler->Initialize(BodyParts[EBodyPartType::RightFist]);
		RightGrabComp->SetGrabHandler(RightHandler);
	}
	else
	{
		UE_LOG(LogInitialize, Warning, TEXT("Grab Components not initialized."));
	}
}

void ADFCharacter::ApplyPhysicalAnimationSettings()
{
	if (!PhysicalAnimComp || !GetMesh()) return;
	
	PhysicalAnimComp->SetSkeletalMeshComponent(GetMesh());
	PhysicalAnimComp->ApplyPhysicalAnimationSettingsBelow(PhysicalAnimStartBone, PhysicalAnimData, false);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PhysicalAnimStartBone, true, false);
}

void ADFCharacter::RegisterAbilities()
{
	AbilityManager->ClearAllAbilities();
	AbilityManager->InitializeAbilities();
}

void ADFCharacter::UpdateSpringArmOrientation()
{
	if (!SpringArm) return;

	const FVector CharacterUp = GetActorUpVector();
	const FVector CameraForward = FRotationMatrix(FRotator(0.f, SpringYaw, 0.f)).GetUnitAxis(EAxis::X);
	const FVector AdjustedForward = FVector::VectorPlaneProject(CameraForward, CharacterUp).GetSafeNormal();

	const FRotator CameraRot = FRotationMatrix::MakeFromXZ(AdjustedForward, CharacterUp).Rotator();
	const FRotator FinalRot = CameraRot + FRotator(-25.f, 0.f, 0.f);

	SpringArm->SetWorldRotation(FinalRot);
}

void ADFCharacter::Initialize()
{
	RegisterAbilities();
	
	HP = MaxHP;

	LeftGrabComp->Released();
	RightGrabComp->Released();
	
	StateManager->SetState(NewObject<UIdleState>(this));
}

void ADFCharacter::Move(const FInputActionValue& Value)
{
	if (StateManager->IsCurrentState(ECharacterStateType::Stunned)) return;
	
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

void ADFCharacter::StartSprint(const FInputActionValue& Value)
{
	if (StateManager->IsCurrentState(ECharacterStateType::Stunned) ||
		StateManager->IsCurrentState(ECharacterStateType::Recover)
		) return;

	GetCharacterMovement()->MaxWalkSpeed = 800.f;
}

void ADFCharacter::StopSprint(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void ADFCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookValue = Value.Get<FVector2D>();
	
	if (!SpringArm) return;
	
	FRotator CurrentRotation = SpringArm->GetRelativeRotation();
	if (LookValue.X != 0.0f)
	{
		CurrentRotation.Yaw += LookValue.X;
	}
	
	SpringArm->SetRelativeRotation(CurrentRotation);

	//const FVector2D LookValue = Value.Get<FVector2D>();
	//if (!SpringArm) return;
//
	//// 누적 Yaw 입력만 처리 (Pitch는 고정)
	//if (LookValue.X != 0.0f)
	//{
	//	SpringYaw += LookValue.X;
	//}
//
	//UpdateSpringArmOrientation();
}


void ADFCharacter::StartGrab(const FInputActionValue& Value)
{
	Server_StartGrab();
}

void ADFCharacter::Server_StartGrab_Implementation()
{
	if (!StateManager->IsCurrentState(ECharacterStateType::Idle)) return;

	if (!BodyParts.Contains(EBodyPartType::LeftFist) || !BodyParts.Contains(EBodyPartType::RightFist)) return;
	
	if (LeftGrabComp && LeftGrabComp->GetCurrentGrabState() != EGrabState::Grabbing)
	{
		LeftGrabComp->StartGrab();
	}

	if (RightGrabComp && RightGrabComp->GetCurrentGrabState() != EGrabState::Grabbing)
	{
		RightGrabComp->StartGrab();
	}
}

void ADFCharacter::StopGrab(const FInputActionValue& Value)
{
	Server_StopGrab();
}

void ADFCharacter::ReleaseGrab(const FInputActionValue& Value)
{
	Server_ReleaseGrab();
}

void ADFCharacter::Server_ReleaseGrab_Implementation()
{
	if (!BodyParts.Contains(EBodyPartType::LeftFist) || !BodyParts.Contains(EBodyPartType::RightFist)) return;

	if (LeftGrabComp && LeftGrabComp->GetCurrentGrabState() == EGrabState::Grabbing)
	{
		LeftGrabComp->Released();
	}

	if (RightGrabComp && RightGrabComp->GetCurrentGrabState() == EGrabState::Grabbing)
	{
		RightGrabComp->Released();
	}

	if (StateManager->IsCurrentState(ECharacterStateType::Grabbed))
		StateManager->SetState(NewObject<UIdleState>(this));
}

void ADFCharacter::Server_StopGrab_Implementation()
{
	//if (StateManager->IsCurrentState(ECharacterStateType::Stunned)) return;

	if (!BodyParts.Contains(EBodyPartType::LeftFist) || !BodyParts.Contains(EBodyPartType::RightFist)) return;
	
	if (LeftGrabComp && LeftGrabComp->GetCurrentGrabState() != EGrabState::Grabbing) LeftGrabComp->StopGrab();

	if (RightGrabComp  && RightGrabComp->GetCurrentGrabState() != EGrabState::Grabbing) RightGrabComp->StopGrab();
}

void ADFCharacter::Headbutt(const FInputActionValue& Value)
{
	Server_Headbutt();
}

void ADFCharacter::Server_Headbutt_Implementation()
{
	if (StateManager->IsCurrentState(ECharacterStateType::Stunned)) return;
	
	if (!BodyParts.Contains(EBodyPartType::Head) || !BodyParts[EBodyPartType::Head]) return;
	
	AbilityManager->StartAbility(TEXT("Headbutt"), BodyParts[EBodyPartType::Head]);
}


void ADFCharacter::StartJump(const FInputActionValue& Value)
{
	if (StateManager->IsCurrentState(ECharacterStateType::Stunned))
	{
		RecoverHandleInput(); // 연타 처리 함수
	}
	else if (
		StateManager->IsCurrentState(ECharacterStateType::Idle) ||
		StateManager->IsCurrentState(ECharacterStateType::Grabbed)
		)
		Super::Jump();
}

void ADFCharacter::BasicAttack(const FInputActionValue& Value)
{
	if (StateManager->IsCurrentState(ECharacterStateType::Idle))
		Server_Punch();
	else if (StateManager->IsCurrentState(ECharacterStateType::Grabbed))
		Server_UseItem();
}

void ADFCharacter::Server_Punch_Implementation()
{
	if (!StateManager->IsCurrentState(ECharacterStateType::Idle)) return;
	
	if (!BodyParts.Contains(EBodyPartType::LeftFist) || !BodyParts.Contains(EBodyPartType::RightFist)) return;

	ABodyPart* Fist = bLeft ? BodyParts[EBodyPartType::LeftFist] : BodyParts[EBodyPartType::RightFist];
	FName AbilityName = bLeft ? TEXT("LeftPunch") : TEXT("RightPunch");
	
	if (!Fist) return;
	AbilityManager->StartAbility(AbilityName, Fist);

	bLeft = !bLeft;	
}

void ADFCharacter::Server_UseItem_Implementation()
{
	if (!StateManager->IsCurrentState(ECharacterStateType::Grabbed)) return;

	AActor* GrabActor = RightGrabComp->GetGrabTargetActor();
	if (!GrabActor) return;
	
	AbilityManager->StartAbility("UseItem", this);
	IGrabbable::Execute_Use(GrabActor);
}

void ADFCharacter::Stun()
{
	StateManager->SetState(NewObject<UStunnedState>(this));
	GetWorldTimerManager().SetTimer(RecoverTimer, this, &ADFCharacter::RecoverStart, 5.f, false);
}

void ADFCharacter::RecoverStart()
{
	if (StateManager->IsCurrentState(ECharacterStateType::Recover)) return;
	
	StateManager->SetState(NewObject<URecoverState>(this));
}

void ADFCharacter::FinishGetUp()
{
	HP = MaxHP;
	StateManager->SetState(NewObject<UIdleState>(this));
 
}

void ADFCharacter::RecoverHandleInput()
{
	if (!StateManager->IsCurrentState(ECharacterStateType::Recover)) return;

	RecoverInputCount++;

	if (RecoverInputCount >= RecoverInputGoal)
	{
		GetWorldTimerManager().ClearTimer(RecoverTimer);
		RecoverStart();
	}
}

void ADFCharacter::SetAllBonesMass(float InMass)
{
	//TArray<FName> BoneNames;
	//GetMesh()->GetBoneNames(BoneNames);
	//for (auto& Name : BoneNames)
	//{
	//	float Mass = GetMesh()->GetBoneMass(Name);
	//	GetMesh()->SetMassOverrideInKg(Name, InMass, true);
	//	UE_LOG(LogInitialize, Log, TEXT("Bone %s Mass: %.2f"), *Name.ToString(), Mass);
	//}
}

float ADFCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                               AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	float DamageApplied = FMath::Min(HP, DamageAmount);
	
	UE_LOG(LogDamaged, Log, TEXT("[%s] 데미지 받음: %.2f, 남은 HP: %.2f (가해자: %s)"),
		*GetName(), DamageApplied, HP - DamageApplied,
		DamageCauser ? *DamageCauser->GetName() : TEXT("알 수 없음"));
	
	if (
		StateManager->IsCurrentState(ECharacterStateType::Stunned) ||
		StateManager->IsCurrentState(ECharacterStateType::Recover)
		) return DamageApplied;
	
	HP -= DamageApplied;

	if (HP <= 0.f)
	{
		HP = 0.f;
		Stun();
	}

	return DamageApplied;
}

AActor* ADFCharacter::GetActualTarget_Implementation()
{
	return this;
}

FVector ADFCharacter::GetResistanceForce_Implementation(AActor* PullingActor)
{
	if (!PullingActor ||
		!IsValid(PullingActor) ||
		(StateManager->IsCurrentState(ECharacterStateType::Stunned))
		)
		return FVector::ZeroVector;
	
	FVector Velocity = GetMovementComponent()->Velocity;
	FVector Direction = (GetActorLocation() - PullingActor->GetActorLocation()).GetSafeNormal();
	// 캐릭터 -> 당기는 액터 방향
	
	const float BaseResistance = 300.0f; // 기본 저항 값
	float Mass = GetMesh()->GetMass();

	// 해당 캐릭터의 이동 방향과 당기는 방향 간의 내적을 계산. (가만히 있으면 0, 같은 방향이면)
	float MovementAgainst = FVector::DotProduct(Velocity.GetSafeNormal(), Direction);
	// 속도에 따라 저항력이 바뀌도록. 
	float DynamicResistance = MovementAgainst * Mass * 50.f;
	
	// 멀어질 수 있는 거리를 정하도록 거리 계산
	float Distance = FVector::Dist(GetActorLocation(), PullingActor->GetActorLocation());
	const float MaxEffectiveDistance = 150.f; // 최대 1.5미터까지 멀어지기 가능
	const float ResistanceMultiplier = 200.f; // 거리 초과시 저항력 증가 배율
		
	// 최대 거리 초과 계산. 최대 거리보다 가까우면 0임.
	float OverDistance = FMath::Max(Distance - MaxEffectiveDistance, 0.f);
	// 최대 거리보다 가까우면 0.
	float DistanceBasedResistance = OverDistance * ResistanceMultiplier;
	
	
	// 기본 저항력 + 동적 저항력 + 거리 비례 저항력
	float TotalResistance = BaseResistance + FMath::Max(DynamicResistance, 0.f) + DistanceBasedResistance;
	
	return Direction * TotalResistance; // 총 저항력 -> 이동 방향에 반대되는 방향으로 적용
}

void ADFCharacter::OnGrabbed_Implementation(AActor* TargetActor)
{
	if (!TargetActor || !MovementModifier) return;
	MovementModifier->RegisterGrabInteraction(TargetActor);
	StateManager->SetState(NewObject<UGrabbedState>(this));
}

void ADFCharacter::OnGrabbedBy_Implementation(AActor* Grabber)
{
	MovementModifier->RegisterGrabInteraction(Grabber);
}

void ADFCharacter::OnGrabReleased_Implementation(AActor* TargetActor)
{
	if (!TargetActor || !MovementModifier) return;
	MovementModifier->UnregisterGrabInteraction(TargetActor);
}

void ADFCharacter::OnGrabReleasedBy_Implementation(AActor* Grabber)
{
	MovementModifier->UnregisterGrabInteraction(Grabber);
}

UPrimitiveComponent* ADFCharacter::GetRoot_Implementation()
{
	return GetMesh();
}