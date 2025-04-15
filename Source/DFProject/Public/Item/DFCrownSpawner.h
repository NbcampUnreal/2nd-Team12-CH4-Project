// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DFCrownSpawner.generated.h"

class ADFCrownActor;

UCLASS()
class DFPROJECT_API ADFCrownSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ADFCrownSpawner();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrownSpawner")
	TSubclassOf<ADFCrownActor> CrownActor;

	UFUNCTION(BlueprintCallable, Category = "CrownSpawner")
	void SpawnCrown();

protected:
	virtual void BeginPlay() override;

};
