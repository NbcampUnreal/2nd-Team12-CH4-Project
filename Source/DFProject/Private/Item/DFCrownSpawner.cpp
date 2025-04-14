#include "Item/DFCrownSpawner.h"
#include "Item/DFCrownActor.h"
#include "Item/DFWearableItem.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

ADFCrownSpawner::ADFCrownSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADFCrownSpawner::BeginPlay()
{
	Super::BeginPlay();	
}

void ADFCrownSpawner::SpawnCrown()
{
	TArray<AActor*> SpawnPoints;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), SpawnPoints);

	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("타깃포인트 없음"));
		return;
	}

	int32 RandIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);

	FVector SpawnLocation = SpawnPoints[RandIndex]->GetActorLocation();

	ADFCrownActor* Crown = GetWorld()->SpawnActor<ADFCrownActor>(CrownActor, SpawnLocation, FRotator::ZeroRotator);

	Crown->ItemMesh->SetSimulatePhysics(true);
}