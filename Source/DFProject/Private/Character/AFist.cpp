// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BodyPart//AFist.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values
AFist::AFist()
{

}

// Called when the game starts or when spawned
void AFist::BeginPlay()
{
	Super::BeginPlay();
}

void AFist::Punch()
{	
	ApplyImpulse();
}


void AFist::Grab(AActor* Target)
{
	// 피직스 컨스트레인트
}

void AFist::Release()
{
}