// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/Grab/Grabbable.h"
#include "GameFramework/Actor.h"
#include "BodyPart.generated.h"

class UAbilityStrategy;
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
	Body,
	LeftFoot,
	RightFoot
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DFPROJECT_API ABodyPart : public AActor, public IGrabbable
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
	
	UFUNCTION(BlueprintCallable, Category="BodyPart")
	virtual void ApplyImpulse();
	
	UFUNCTION(BlueprintCallable, Category="Attack")
	void PerformAttack();

	UFUNCTION(BlueprintCallable, Category="Attack")
	void SetAttackStrategy(UAbilityStrategy* NewStrategy);
	
	UFUNCTION(BlueprintCallable, Category="Attack")
	void SaveAttackTime();
	
protected:	
	FTransform GetOffsetTransform(const ACharacter* TargetCharacter, const UAttachInfoComponent* AttachInfo);

	UFUNCTION()
	void OnAttackOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	UPROPERTY(EditAnywhere)
	float ImpulsePower = 10000.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Collider")
	TObjectPtr<USphereComponent> BodyCollider;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPhysicsConstraintComponent> BoneConstraint;
	
	FName BoneToAttach;

	UPROPERTY(EditAnywhere, Category="Collider")
	float VirtualMass = 5.0f;

	UPROPERTY(EditAnywhere, Category="Attack")
	UAbilityStrategy* CurrentAttackStrategy;
	
	float LastAttackTime = -1.f;
	
	UPROPERTY(EditDefaultsOnly)
	float AttackValidDuration = 0.5f;
	
	UPROPERTY()
	TObjectPtr<ACharacter> OwningCharacter;
};
