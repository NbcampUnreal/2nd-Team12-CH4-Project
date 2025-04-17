#include "Item/DFShopItemSpawner.h"
#include "Item/DFWearableItem.h"
#include "Item/DFWearableItemActor.h"
#include "Item/DFItemBaseActor.h"
#include "Character/DFCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"
#include "Engine/TargetPoint.h"

ADFShopItemSpawner::ADFShopItemSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Scene);

	
	PlayerCharacter = nullptr;
}

void ADFShopItemSpawner::BeginPlay()
{
	Super::BeginPlay();	

	TArray<AActor*> TempArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), TempArray);

	for (AActor* Actor : TempArray)
	{
		if (!IsValid(Actor) || Actor->IsPendingKillPending())
		{
			continue;
		}

		SpawnPoints.Add(Actor);
	}

	SpawnPoints.Sort([](const AActor& A, const AActor& B)
		{
			return A.GetActorLabel() < B.GetActorLabel();
		});
}

void ADFShopItemSpawner::SpawnItemToShop()
{
	FPrimaryAssetType ItemType = FPrimaryAssetType("WearableItem");

	TArray<FPrimaryAssetId> AssetIds;
	UAssetManager::Get().GetPrimaryAssetIdList(ItemType, AssetIds);

	if (AssetIds.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("스포너 : 애셋 없음"))
			return;
	}

	AssetIds.Sort([](const FPrimaryAssetId& A, const FPrimaryAssetId& B)
		{
			return A.PrimaryAssetName.LexicalLess(B.PrimaryAssetName);
		});

	for (int i = 0; i < AssetIds.Num(); i++)
	{
		UAssetManager::Get().LoadPrimaryAsset(AssetIds[i], {}, FStreamableDelegate::CreateLambda([=, this]() {
			UObject* Loaded = UAssetManager::Get().GetPrimaryAssetObject(AssetIds[i]);
			UDFWearableItem* LoadedItem = Cast<UDFWearableItem>(Loaded);

			if (!LoadedItem)
			{
				UE_LOG(LogTemp, Warning, TEXT("스포너 : 데이터 없음"));
				return;
			}

			if (!LoadedItem->bIsUnlocked)
			{
				return;
			}

			if (!SpawnPoints.IsValidIndex(i) || !SpawnPoints[i])
			{
				return;
			}

			FString Name = SpawnPoints[i]->GetActorLabel();

			FTransform SpawnTransform = SetSpawnTransform(i);

			ADFWearableItemActor* SpawnedActor = GetWorld()->SpawnActor<ADFWearableItemActor>(LoadedItem->ItemActorClass, SpawnTransform);

			if (!SpawnedActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("액터 스폰 실패"));
				return;
			}

			SpawnedActor->SetActor(LoadedItem);
			}));
	}
}

FTransform ADFShopItemSpawner::SetSpawnTransform(int32 LocationIndex)
{
	FVector SpawnLocation = SpawnPoints[LocationIndex]->GetActorLocation();

	return FTransform(FRotator::ZeroRotator, SpawnLocation);
}

void ADFShopItemSpawner::EquipItemToCharacter()
{

}
