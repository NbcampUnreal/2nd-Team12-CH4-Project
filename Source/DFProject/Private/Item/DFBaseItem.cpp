// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DFBaseItem.h"

// Sets default values
ADFBaseItem::ADFBaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADFBaseItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADFBaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

