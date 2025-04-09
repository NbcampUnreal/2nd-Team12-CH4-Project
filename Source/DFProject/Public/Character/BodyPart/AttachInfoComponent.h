// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AttachInfoComponent.generated.h"

class ABodyPart;
class USphereComponent;
enum class EBodyPartType : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DFPROJECT_API UAttachInfoComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UAttachInfoComponent();
	
	// 오프셋은 타겟 본 위치와 이 컴포넌트의 위치의 차이로 구함.
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attach Info")
	FName TargetBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attach Info")
	TSubclassOf<ABodyPart> BodyPartClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attach Info")
	EBodyPartType BodyPartType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attach Info")
	bool bAutoSpawnBeginPlay = true;
	
	UPROPERTY(EditAnywhere, Category="Attach Info")
	TObjectPtr<USphereComponent> PreviewCollider;
};
