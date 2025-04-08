// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "DFCharacter.generated.h"

class UPhysicalAnimationComponent;
class AFist;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class ABodyPart;
struct FInputActionValue;

enum class EBodyPartType : uint8;

UCLASS()
class DFPROJECT_API ADFCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADFCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;  
public:	

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	
	UFUNCTION(Server, Reliable) // 반드시 반영해야해서 Reliable로 했는데 잘 모르겠음.
	void Server_Move(const FRotator& Rotation);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_Move(const FRotator& Rotation);

	
	void Punch(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_Punch();

	
	void Sprint(const FInputActionValue& Value); //  CharacterMovement의 스피드 올리기 (이건 자동 리플), 달리기 이펙트 생성
	
	void Look(const FInputActionValue& Value); // 다른 클라는 몰라도 되니까 로컬만

	void Grab(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_Grab();
	
	

	void DropKick(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DropKick();
	
	
	void StartJump(const FInputActionValue& Value);

	
	void Headbutt(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_Headbutt();
	
	
	UFUNCTION(BlueprintCallable, Category="PhysicalAnimation")
	void SpawnBodyParts();

	UFUNCTION(BlueprintCallable, Category="PhysicalAnimation")
	void ApplyPhysicalAnimationSettings();

	UFUNCTION(BlueprintCallable, Category="PhysicalAnimation")
	void Stun();
	
	UFUNCTION(BlueprintCallable, Category="PhysicalAnimation")
	void RecoverFromStun();

	UFUNCTION(BlueprintCallable, Category="PhysicalAnimation")
	void FinishGetUp();
	
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
	TObjectPtr<UInputAction> DropKickAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	TObjectPtr<UInputAction> TossAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	TObjectPtr<UInputAction> HeadbuttAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Input)
	TSubclassOf<AFist> FistClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Fist")
	FName LeftHandBoneName = TEXT("LeftHand");

	UPROPERTY(EditDefaultsOnly, Category="Fist")
	FName RightHandBoneName = TEXT("RightHand");
	
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

	UPROPERTY()
	FQuat InitialRecoveryRotation;

	bool bLeft = true;
	bool bIsStunned = false;
	bool bIsRecovering = false;
	float RecoverAlpha = 0.0f;
	float RecoverSpeed = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animations")
	UAnimMontage* GetUpFrontMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animations")
	UAnimMontage* GetUpBackMontage;
};








