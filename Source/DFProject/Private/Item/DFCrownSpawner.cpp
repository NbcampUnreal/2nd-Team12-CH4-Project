#include "Item/DFCrownSpawner.h"
#include "Item/DFCrownActor.h"
#include "Item/DFWearableItem.h"
#include "Components/BoxComponent.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

ADFCrownSpawner::ADFCrownSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Scene);

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SpawnArea->SetupAttachment(Scene);
	SpawnArea->SetBoxExtent(FVector(50.0f, 50.0f, 10.0f));
}

void ADFCrownSpawner::BeginPlay()
{
	Super::BeginPlay();	
}

void ADFCrownSpawner::SpawnCrown()
{
	FVector BoxExtent = SpawnArea->GetScaledBoxExtent();

	FVector BoxLocation = SpawnArea->GetComponentLocation();

	FVector SpawnLocation = BoxLocation + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		BoxExtent.Z
	);

	Crown = GetWorld()->SpawnActor<ADFCrownActor>(CrownActor, SpawnLocation, FRotator::ZeroRotator);

}