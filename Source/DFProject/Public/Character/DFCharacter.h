// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/Grab/Grabbable.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "DFCharacter.generated.h"

class ADFItemBaseActor;
class UAbilityStrategyManager;
class UCharacterStateManager;
class UMovementModifierComponent;
class UGrabComponent;
class UPhysicalAnimationComponent;
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
	ADFCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;  
public:
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	ABodyPart* GetBodyPart(EBodyPartType Type);
	
	UFUNCTION(BlueprintCallable)
	void DeadEvent();

	UFUNCTION(BlueprintCallable)
	ADFItemBaseActor* GetCurrentItem();
	
	////// 캐릭터 세팅
	UFUNCTION(BlueprintCallable, Category="Initialize")
	void ReadyToPlay();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category="Initialize")
	void Multicast_ReadyToPlay();
	
	UFUNCTION(BlueprintCallable, Category="PhysicalAnimation")
	void SpawnBodyParts();
	
	UFUNCTION(BlueprintCallable, Category="PhysicalAnimation")
	void ApplyPhysicalAnimationSettings();

	void RegisterAbilities();
	
	UFUNCTION(BlueprintCallable, Category="Respawn")
	void Initialize();
	///////
	
	////// 캐릭터 액션
	void Move(const FInputActionValue& Value);
	
	UFUNCTION(Server, UnReliable) // 반드시 반영해야해서 Reliable로 했는데 잘 모르겠음.
	void Server_Move(const FRotator& Rotation);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_Move(const FRotator& Rotation);
	
	void BasicAttack(const FInputActionValue& Value);

	UFUNCTION(Server, UnReliable, BlueprintCallable)
	void Server_Punch();
	
	UFUNCTION(Server, UnReliable, BlueprintCallable)
	void Server_UseItem();
	
	void StartSprint(const FInputActionValue& Value); //  CharacterMovement의 스피드 올리기 (이건 자동 리플), 달리기 이펙트 생성?

	void StopSprint(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value); // 다른 클라는 몰라도 되니까 로컬만

	void StartGrab(const FInputActionValue& Value);

	UFUNCTION(Server, UnReliable, BlueprintCallable) // 어빌리티로 이동 예정
	void Server_StartGrab();

	void StopGrab(const FInputActionValue& Value);
	
	UFUNCTION(Server, UnReliable, BlueprintCallable)
	void Server_StopGrab();

	void ReleaseGrab(const FInputActionValue& Value);

	UFUNCTION(Server, UnReliable, BlueprintCallable)
	void Server_ReleaseGrab();
	
	void StartJump(const FInputActionValue& Value);
	
	void Headbutt(const FInputActionValue& Value);

	UFUNCTION(Server, UnReliable, BlueprintCallable)
	void Server_Headbutt();
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

	// 잡은 대상의 진짜 owner
	virtual AActor* GetActualTarget_Implementation() override;

	// Movement Modifier에서 저항력 계산에 사용
	virtual FVector GetResistanceForce_Implementation(AActor* PullingActor) override;

	// 잡혔을 때
	virtual void OnGrabbed_Implementation(AActor* TargetActor) override;

	virtual void OnGrabbedBy_Implementation(AActor* Grabber, UPhysicsConstraintComponent* InGrabberConstraint) override;

	virtual void OnGrabReleased_Implementation(AActor* TargetActor) override;

	virtual void OnGrabReleasedBy_Implementation(AActor* Grabber) override;

	virtual UPrimitiveComponent* GetRoot_Implementation() override;

	virtual void DestroyThis_Implementation() override;
	
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

	TArray<ABodyPart*> BodyParts;

	UPROPERTY(ReplicatedUsing=OnRep_LeftFist)
	ABodyPart* LeftFist;

	UPROPERTY(ReplicatedUsing=OnRep_RightFist)
	ABodyPart* RightFist;

	UFUNCTION()
	void OnRep_LeftFist();

	UFUNCTION()
	void OnRep_RightFist();
	
	// 바디 파츠 붙일 때 사용하는 변수. 클라는 복제된 값을 사용하기에 몰라도 된다.
	UPROPERTY()
	FTransform MeshOffset;

	bool bLeft = true; // 주먹질 왼손?

	float MaxHP = 100.0f;
	// 체력바 없으면 복제할 필요가 있을까?
	float HP = 100.0f;

	// 서버만 가져도 됨. 기절 회복 로직은 서버가 가지면 되기에 클라는 이 변수들을 몰라도 됌
	FTimerHandle RecoverTimer;
	int32 RecoverInputCount = 0;
	int32 RecoverInputGoal = 0;

	// 잡기 로직도 서버에서만 실행해도 될 듯.
	// 손을 움직일 때 body parts에 force를 주는 멀티 캐스트 함수만 있으면 될 것
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGrabComponent> RightGrabComp;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGrabComponent> LeftGrabComp;

	// 현재 상태에 따라 플레이어의 이동을 수정하기
	// CharacterMovementComp를 건드니 서버만 가지고 있는게 좋을 듯
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMovementModifierComponent> MovementModifier;

	// 얘도 서버만 가지도록. 스킬 사용은 서버에서만
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAbilityStrategyManager> AbilityManager;

	// 나중에 이펙트 추가?
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCharacterStateManager> StateManager;
};





