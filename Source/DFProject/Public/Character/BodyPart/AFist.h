// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BodyPart.h"
#include "GameFramework/Actor.h"
#include "AFist.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DFPROJECT_API AFist : public ABodyPart
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFist();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintCallable, Category="Fist")
	void Punch();
	
	UFUNCTION(BlueprintCallable, Category="Fist")
	void Grab(AActor* Target);

	UFUNCTION(BlueprintCallable, Category="Fist")
	void Release();
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UPhysicsConstraintComponent> GrabConstraint;
};
