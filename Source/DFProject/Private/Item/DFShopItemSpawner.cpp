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

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), SpawnPoints);

	SpawnPoints.Sort([](const AActor& A, const AActor& B)
		{
			return A.GetName() < B.GetName();
		});
}

void ADFShopItemSpawner::SpawnItemToShop()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("SpawnItemToShop")));

	FPrimaryAssetType ItemType = FPrimaryAssetType("WearableItem");

	TArray<FPrimaryAssetId> AssetIds;
	UAssetManager::Get().GetPrimaryAssetIdList(ItemType, AssetIds);

	if (AssetIds.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("스포너 : 애셋 없음"))
			return;
	}

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
