// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DFCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
struct FInputActionValue;
class UInputAction;

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
	void Sprint(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Grab(const FInputActionValue& Value);
	void Kick(const FInputActionValue& Value);
	void StartJump(const FInputActionValue& Value);

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
	TObjectPtr<UInputAction> KickAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	TObjectPtr<UInputAction> TossAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	TObjectPtr<UInputAction> HeadbuttAction;
};


