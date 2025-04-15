// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/Grab/Grabbable.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "DFCharacter.generated.h"

class UGravityMovementComponent;
class UAbilityStrategyManager;
class UCharacterStateManager;
class UMovementModifierComponent;
class UGrabComponent;
class UPhysicalAnimationComponent;
class AFist;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class ABodyPart;
struct FInputActionValue;

enum class EBodyPartType : uint8;

DECLARE_LOG_CATEGORY_EXTERN(LogDamaged, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogInitialize, Log, All);

UCLASS()
class DFPROJECT_API ADFCharacter : public ACharacter, public IGrabbable
{
	GENERATED_BODY()

public:
	ADFCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;  
public:
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	bool EventOnDestroy();

	UFUNCTION(BlueprintCallable)
	UGravityMovementComponent* GetGravityMovementComponent();

	
	UFUNCTION(BlueprintCallable)
	void UpdateSpringArmOrientation();

	float SpringYaw = 0.f;
	////// 캐릭터 액션
	void Move(const FInputActionValue& Value);
	
	UFUNCTION(Server, Reliable) // 반드시 반영해야해서 Reliable로 했는데 잘 모르겠음.
	void Server_Move(const FRotator& Rotation);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_Move(const FRotator& Rotation);
	
	void BasicAttack(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_Punch();
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_UseItem();
	
	
	void StartSprint(const FInputActionValue& Value); //  CharacterMovement의 스피드 올리기 (이건 자동 리플), 달리기 이펙트 생성

	void StopSprint(const FInputActionValue& Value); //  CharacterMovement의 스피드 올리기 (이건 자동 리플), 달리기 이펙트 생성
	
	void Look(const FInputActionValue& Value); // 다른 클라는 몰라도 되니까 로컬만

	void StartGrab(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_StartGrab();

	void StopGrab(const FInputActionValue& Value);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_StopGrab();

	void ReleaseGrab(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ReleaseGrab();
	
	void StartJump(const FInputActionValue& Value);
	
	void Headbutt(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_Headbutt();
	///////

	
	////// 캐릭터 세팅
	UFUNCTION(BlueprintCallable, Category="PhysicalAnimation")
	void SpawnBodyParts();
	
	UFUNCTION(BlueprintCallable, Category="PhysicalAnimation")
	void ApplyPhysicalAnimationSettings();

	void RegisterAbilities();

	UFUNCTION(BlueprintCallable, Category="Respawn")
	void Initialize();
	///////
	

	///// 스턴 관련 함수
	UFUNCTION(BlueprintCallable, Category="PhysicalAnimation")
	void Stun();
	
	UFUNCTION(BlueprintCallable, Category="PhysicalAnimation")
	void RecoverStart();

	UFUNCTION(BlueprintCallable, Category="PhysicalAnimation")
	void FinishGetUp();

	void RecoverHandleInput();

	void SetAllBonesMass(float InMass);
	///////
	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual AActor* GetActualTarget_Implementation() override;

	virtual FVector GetResistanceForce_Implementation(AActor* PullingActor) override;

	virtual void OnGrabbed_Implementation(AActor* TargetActor) override;

	virtual void OnGrabbedBy_Implementation(AActor* Grabber) override;

	virtual void OnGrabReleased_Implementation(AActor* TargetActor) override;

	virtual void OnGrabReleasedBy_Implementation(AActor* Grabber) override;

	virtual UPrimitiveComponent* GetRoot_Implementation() override;


	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Camera)
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Camera)
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	TObjectPtr<UInputAction> PunchAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	TObjectPtr<UInputAction> GrabAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	TObjectPtr<UInputAction> ReleaseGrabAction;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	TObjectPtr<UInputAction> TossAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	TObjectPtr<UInputAction> HeadbuttAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=PhysicalAnimation)
	TObjectPtr<UPhysicalAnimationComponent> PhysicalAnimComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=PhysicalAnimation)
	FPhysicalAnimationData PhysicalAnimData;
	
	UPROPERTY(EditDefaultsOnly, Category=PhysicalAnimation)
	FName PhysicalAnimStartBone = TEXT("Hips");

	UPROPERTY()
	TMap<EBodyPartType, ABodyPart*> BodyParts;

	UPROPERTY()
	FTransform MeshOffset;

	bool bLeft = true;

	float MaxHP = 100.0f;
	float HP = 100.0f;

	FTimerHandle RecoverTimer;
	int32 RecoverInputCount = 0;
	int32 RecoverInputGoal = 0;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGrabComponent> RightGrabComp;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGrabComponent> LeftGrabComp;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMovementModifierComponent> MovementModifier;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAbilityStrategyManager> AbilityManager;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCharacterStateManager> StateManager;
};





