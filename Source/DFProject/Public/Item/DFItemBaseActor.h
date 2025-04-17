// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/DFItemSpawner.h"
#include "DFItemBaseActor.generated.h"

//class UDFItemInstance;
class USphereComponent;
class UDFItemAbilityComponent;
class UPhysicalAnimationComponent;

UCLASS()
class DFPROJECT_API ADFItemBaseActor : public AActor
{
	GENERATED_BODY()
	
public:
	ADFItemBaseActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	USkeletalMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	USphereComponent* GripArea;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TArray<UDFItemAbilityComponent*> ItemAbilities;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	UPhysicalAnimationComponent* PhysicalAnimComp;

	UPROPERTY(ReplicatedUsing = OnRep_ItemData)
	FItemInstanceData ItemData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	bool bCanBeGrabbed;


	UFUNCTION()
	virtual void OnGripAreaBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnGripAreaEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	void OnRep_ItemData();

	UFUNCTION(BlueprintCallable, Category = "Item")
	void AbilitiesMainAction();

	virtual void SetupItem(const FItemInstanceData& InData);

protected:
	virtual void BeginPlay() override;

};
