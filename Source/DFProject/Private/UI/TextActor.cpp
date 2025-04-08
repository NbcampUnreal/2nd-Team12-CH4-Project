// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TextActor.h"
#include "Text3DComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
ATextActor::ATextActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollsionBox"));
	CollisionComp->SetupAttachment(RootComp);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	Text3DComp = CreateDefaultSubobject<UText3DComponent>(TEXT("Text3DComponent"));
	Text3DComp->SetupAttachment(CollisionComp);
}

void ATextActor::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();

	if (bActiveHover)
	{
		AddActorLocalRotation(FRotator(0, HoverRotation, 0));
	}
}

void ATextActor::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();

	if (bActiveHover)
	{
		AddActorLocalRotation(FRotator(0, -HoverRotation, 0));
	}
}

