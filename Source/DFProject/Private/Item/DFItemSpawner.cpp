#include "Item/DFItemSpawner.h"
#include "Item/DFItemBaseActor.h"
#include "Item/DFBattleItem.h"
#include "Item/DFItemInstance.h"
#include "Components/BoxComponent.h"
#include "Engine/AssetManager.h"

ADFItemSpawner::ADFItemSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Scene);

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpanwAreaCollision"));
	SpawnArea->SetupAttachment(Scene);
	SpawnArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnArea->SetBoxExtent(FVector(100.0f), false);

}

void ADFItemSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

FTransform ADFItemSpawner::SetSpawnTransform()
{
	FVector BoxExtent = SpawnArea->GetScaledBoxExtent();
	FVector BoxLocation = SpawnArea->GetComponentLocation();
	FVector SpawnLocation = BoxLocation + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);

	FRotator RandRotation = FRotator(FMath::FRandRange(0.0f, 360.f), FMath::FRandRange(0.0f, 360.f), FMath::FRandRange(0.0f, 360.f));

	return FTransform(RandRotation, SpawnLocation);
}

void ADFItemSpawner::SpawnItem()
{
	FPrimaryAssetType ItemType = FPrimaryAssetType("BattleItem");

	TArray<FPrimaryAssetId> AssetIds;
	UAssetManager::Get().GetPrimaryAssetIdList(ItemType, AssetIds);

	if (AssetIds.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("NoAsset"))
			return;
	}
	int32 RandIndex = FMath::RandRange(0, AssetIds.Num() - 1);
	FPrimaryAssetId ChosenId = AssetIds[RandIndex];

	UAssetManager::Get().LoadPrimaryAsset(ChosenId, {}, FStreamableDelegate::CreateLambda([=, this]() {
		UObject* Loaded = UAssetManager::Get().GetPrimaryAssetObject(ChosenId);
		UDFBattleItem* LoadedItem = Cast<UDFBattleItem>(Loaded);
	
		if (!LoadedItem || !LoadedItem->ItemActorClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("NoData Or NoActorClass"));
			return;			
		}

		UDFItemInstance* NewInstance = NewObject<UDFItemInstance>();
		NewInstance->Initialize(LoadedItem);

		FTransform SpawnTransform = SetSpawnTransform();

		ADFItemBaseActor* SpawnedActor = GetWorld()->SpawnActor<ADFItemBaseActor>(LoadedItem->ItemActorClass, SpawnTransform);

		if (!SpawnedActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed Spawn Actor"));
			return;
		}

		SpawnedActor->SetupItem(NewInstance);

	}));
}


