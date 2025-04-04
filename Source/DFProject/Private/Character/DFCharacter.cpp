// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DFCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ADFCharacter::ADFCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ADFCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
	
}

void ADFCharacter::Move(const FInputActionValue& Value)
{
	AController* PlayerController = GetController();

	if (!ensure(PlayerController)) return;

	const FVector2D MoveValue = Value.Get<FVector2D>();
	const FRotator MovementRotation(0.f, PlayerController->GetControlRotation().Yaw, 0.0f);

	FVector MovementDirection = FVector::ZeroVector;

	if (MoveValue.X != 0.0f)
	{
		MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(MovementDirection, MoveValue.X);
	}

	if (MoveValue.Y != 0.0f)
	{
		MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(MovementDirection, MoveValue.Y);
	}

	// 🔥 이동 방향이 0이 아니라면 캐릭터 회전 적용 (부드럽게)
	if (!MovementDirection.IsNearlyZero())
	{
		FRotator TargetRotation = MovementDirection.Rotation();
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 10.0f);
		SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f)); // Yaw 회전만 적용
	}
}

void ADFCharacter::Sprint(const FInputActionValue& Value)
{
}

void ADFCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookValue = Value.Get<FVector2D>();

	if (!SpringArm) return; // 스프링암이 없으면 실행 안 함

	FRotator CurrentRotation = SpringArm->GetRelativeRotation();

	// X축(Yaw): 좌우 회전 (캐릭터 회전과 분리)
	if (LookValue.X != 0.0f)
	{
		CurrentRotation.Yaw += LookValue.X;
	}

	// Y축(Pitch): 위아래 회전 (카메라 위아래 움직임)
	if (LookValue.Y != 0.0f)
	{
		CurrentRotation.Pitch = FMath::Clamp(CurrentRotation.Pitch + LookValue.Y, -80.0f, 80.0f); // 상하 제한
	}

	SpringArm->SetRelativeRotation(CurrentRotation);
}
void ADFCharacter::Grab(const FInputActionValue& Value)
{
}

void ADFCharacter::Kick(const FInputActionValue& Value)
{
}

void ADFCharacter::StartJump(const FInputActionValue& Value)
{
	Jump();
}