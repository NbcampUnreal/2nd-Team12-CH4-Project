// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BodyPart.generated.h"

class UAttachInfoComponent;
class ADFCharacter;
class UPhysicsConstraintComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EBodyPartType : uint8
{
	None,
	LeftFist,
	RightFist,
	Head,
	LeftFoot,
	RightFoot
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DFPROJECT_API ABodyPart : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABodyPart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category="BodyPart")
	virtual void Attach(ACharacter* TargetCharacter, const UAttachInfoComponent* AttachInfo);
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_ApplyImpulse();
	
	UFUNCTION(BlueprintCallable, Category="BodyPart")
	virtual void ApplyImpulse();
protected:	

	FTransform GetOffsetTransform(const ACharacter* TargetCharacter, const UAttachInfoComponent* AttachInfo);
	
	UPROPERTY(EditAnywhere)
	float ImpulsePower = 10000.0f;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USphereComponent> BodyCollider;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPhysicsConstraintComponent> HandConstraint;
	
	FName BoneToAttach;
	
	UPROPERTY()
	TObjectPtr<ACharacter> OwningCharacter;
};
