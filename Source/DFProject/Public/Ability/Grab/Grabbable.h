// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grabbable.generated.h"

UENUM(BlueprintType)
enum class EGrabMode : uint8
{
	LooseAttach      UMETA(DisplayName = "Loose Attach"),      // 단순히 잡기만
	AttachOnly       UMETA(DisplayName = "Attach Only"),       // 소켓에 붙지만 활성화 없음
	AttachAndEnable  UMETA(DisplayName = "Attach and Enable")  // 소켓에 붙고, 기능 활성화
};

UINTERFACE(MinimalAPI, Blueprintable)
class UGrabbable : public UInterface
{
	GENERATED_BODY()
};

class DFPROJECT_API IGrabbable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grab")
	TArray<FName> GetPreferredGrabSocketNames() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grab")
	EGrabMode GetGrabMode() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grab")
	void OnGrabbedBy(AActor* Grabber);
};