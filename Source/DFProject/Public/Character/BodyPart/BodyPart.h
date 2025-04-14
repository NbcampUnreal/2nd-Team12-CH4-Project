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
	LeftArm,
	RightArm,
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
	ABodyPart();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category="BodyPart")
	virtual void Attach(ACharacter* TargetCharacter, const UAttachInfoComponent* AttachInfo);
	
	UFUNCTION(BlueprintCallable, Category="Attack")
	void PerformAttack();

	UFUNCTION(BlueprintCallable, Category="Attack")
	void SetAttackStrategy(UAbilityStrategy* NewStrategy);
	
	UFUNCTION(BlueprintCallable, Category="Attack")
	void SaveAttackTime();

	TObjectPtr<USphereComponent> GetBodyCollider();
	TObjectPtr<ACharacter> GetOwningCharacter();

	virtual AActor* GetActualTarget_Implementation() override;
	virtual FVector GetResistanceForce_Implementation(AActor* PullingActor) override;
	virtual void OnGrabbed_Implementation(AActor* Grabber) override;
	virtual void OnGrabReleased_Implementation(AActor* Grabber) override;
	virtual UPrimitiveComponent* GetRoot_Implementation() override;

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
