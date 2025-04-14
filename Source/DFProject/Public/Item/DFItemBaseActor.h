// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DFItemBaseActor.generated.h"

class UDFItemInstance;
class USphereComponent;
class UDFItemAbilityComponent;
class UPhysicalAnimationComponent;

UCLASS()
class DFPROJECT_API ADFItemBaseActor : public AActor
{
	GENERATED_BODY()
	
public:
	ADFItemBaseActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UDFItemInstance* ItemInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	USphereComponent* GripArea;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TArray<UDFItemAbilityComponent*> ItemAbilities;

	UPROPERTY(VisibleAnywhere, Category = "Physics")
	UPhysicalAnimationComponent* PhysicalAnimComp;


	UFUNCTION()
	void OnGripAreaBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnGripAreaEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual void SetupItem(UDFItemInstance* NewItemInstance);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void AbilitiesMainAction();

	UFUNCTION(BlueprintCallable, Category = "Item")
	FName GetCurrentItemId() const;

protected:
	virtual void BeginPlay() override;

private:
	void AttachAbilities();
	bool bCanBeGrabbed;
};
