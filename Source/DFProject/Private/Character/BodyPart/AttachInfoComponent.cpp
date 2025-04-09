// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BodyPart/AttachInfoComponent.h"

#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"

UAttachInfoComponent::UAttachInfoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	PreviewCollider = CreateDefaultSubobject<USphereComponent>(TEXT("PreviewCollider"));
	PreviewCollider->SetupAttachment(this);
	PreviewCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewCollider->SetHiddenInGame(true);
	PreviewCollider->bIsEditorOnly = true;
}
