// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BodyPart//AFist.h"

#include "Components/SphereComponent.h"
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

void AFist::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AFist::MoveFistToward(const FVector& TargetLocation)
{
	FVector CurrentLocation = GetActorLocation();
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();

	float Strength = 2000.0f;
	FVector Force = Direction * Strength;

	BodyCollider->AddForce(Force, NAME_None, true);

}


void AFist::Grab(AActor* Target)
{
	// 피직스 컨스트레인트
}

void AFist::ReleaseGrab()
{
}