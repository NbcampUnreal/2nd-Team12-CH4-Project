// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DFPlayerController.generated.h"
class UInputMappingContext;

/**
 * 
 */
UCLASS()
class DFPROJECT_API ADFPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ADFPlayerController();

	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input)
	TObjectPtr<UInputMappingContext> IMC;
	
};
