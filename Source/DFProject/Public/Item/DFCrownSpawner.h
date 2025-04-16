// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DFCrownSpawner.generated.h"

class UBoxComponent;
class ADFCrownActor;

UCLASS()
class DFPROJECT_API ADFCrownSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ADFCrownSpawner();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrownSpawner")
	USceneComponent* Scene;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CrownSpawner")
	UBoxComponent* SpawnArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrownSpawner")
	TSubclassOf<ADFCrownActor> CrownActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrownSpawner")
	ADFCrownActor* Crown;

	UFUNCTION(BlueprintCallable, Category = "CrownSpawner")
	void SpawnCrown();

protected:
	virtual void BeginPlay() override;

};
