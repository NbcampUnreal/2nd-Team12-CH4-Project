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

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	
	UFUNCTION(Server, Reliable) // 반드시 반영해야해서 Reliable로 했는데 잘 모르겠음.
	void Server_Move(const FRotator& Rotation);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_Move(const FRotator& Rotation);

	void Punch(const FInputActionValue& Value);
	void Sprint(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Grab(const FInputActionValue& Value);
	void DropKick(const FInputActionValue& Value);
	void StartJump(const FInputActionValue& Value);
	void Headbutt(const FInputActionValue& Value);
	
	void SpawnBodyParts();

	UFUNCTION(BlueprintCallable, Category="PhysicalAnimation")
	void ApplyPhysicalAnimationSettings();

	UFUNCTION(BlueprintCallable, Category="PhysicalAnimation")
	void Stun();
	
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
	
	bool Left = true;
};






