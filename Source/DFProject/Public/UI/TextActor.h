// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TextActor.generated.h"

UCLASS()
class DFPROJECT_API ATextActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATextActor();

protected:
	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;

	/* Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class UArrowComponent* RootComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class UBoxComponent* CollisionComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class UText3DComponent* Text3DComp;

	/* Fields */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	bool bActiveHover = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	float HoverRotation = 30.f;
};
