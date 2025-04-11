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

	bIsValidArea = false;
	SpawnCount = 0;
}

void ADFItemSpawner::SpawnItem()
{
	FPrimaryAssetType ItemType = FPrimaryAssetType("BattleItem");

	TArray<FPrimaryAssetId> AssetIds;
	UAssetManager::Get().GetPrimaryAssetIdList(ItemType, AssetIds);

	if (AssetIds.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("스포너 : 애셋 없음"))
			return;
	}
	int32 RandIndex = FMath::RandRange(0, AssetIds.Num() - 1);
	FPrimaryAssetId ChosenId = AssetIds[RandIndex];

	UAssetManager::Get().LoadPrimaryAsset(ChosenId, {}, FStreamableDelegate::CreateLambda([=, this]() {
		UObject* Loaded = UAssetManager::Get().GetPrimaryAssetObject(ChosenId);
		UDFBattleItem* LoadedItem = Cast<UDFBattleItem>(Loaded);
	
		if (!LoadedItem || !LoadedItem->ItemActorClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("스포너 : 데이터 또는 액터클래스 없음"));
			return;			
		}

		UDFItemInstance* NewInstance = NewObject<UDFItemInstance>();
		NewInstance->Initialize(LoadedItem);

		bIsValidArea = false;
		FTransform SpawnTransform = SetSpawnTransform();
		if (!bIsValidArea)
		{
			return;
		}

		ADFItemBaseActor* SpawnedActor = GetWorld()->SpawnActor<ADFItemBaseActor>(LoadedItem->ItemActorClass, SpawnTransform);

		if (!SpawnedActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("액터 스폰 실패"));
			return;
		}

		SpawnedActor->SetupItem(NewInstance);
	}));
}

FTransform ADFItemSpawner::SetSpawnTransform()
{
	FVector BoxExtent = SpawnArea->GetScaledBoxExtent();
	FVector BoxLocation = SpawnArea->GetComponentLocation();
	FVector SpawnLocation;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;
	Params.AddIgnoredActor(this);

	const float Offset = 200.0f;
	const FVector Directions[4] = {
		FVector(Offset, 0.0f, 0.0f),
		FVector(-Offset, 0.0f, 0.0f),
		FVector(0.0f, Offset, 0.0f),
		FVector(0.0f, -Offset, 0.0f)
	};

	int32 TryCount = 0;

	while (!bIsValidArea && TryCount < 50)
	{
		SpawnLocation = BoxLocation + FVector(
			FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
			FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
			BoxExtent.Z
		);

		FVector LineEnd = SpawnLocation + FVector(0.0f, 0.0f, -3000.0f);

		if (GetWorld()->LineTraceSingleByChannel(Hit, SpawnLocation, LineEnd, ECC_Visibility, Params))
		{
			AActor* HitActor = Hit.GetActor();

			if (HitActor && HitActor->ActorHasTag("ItemSpawnArea"))
			{
				int32 HitCount = 0;

				for (const FVector& OffsetDir : Directions)
				{
					FHitResult NearbyHit;
					FVector Start = Hit.ImpactPoint + OffsetDir + FVector(0.0f, 0.0f, 20);
					FVector End = Hit.ImpactPoint + OffsetDir + FVector(0.0f, 0.0f, -200);

					if (GetWorld()->LineTraceSingleByChannel(NearbyHit, Start, End, ECC_Visibility))
					{
						if (NearbyHit.GetActor() && NearbyHit.GetActor()->ActorHasTag("ItemSpawnArea"))
						{
							HitCount++;
						}
					}

					if (HitCount >= 4)
					{
						break;
					}
				}

				if (HitCount < 4)
				{
					TryCount++;

					if (TryCount >= 50)
					{
						UE_LOG(LogTemp, Warning, (TEXT("스폰 가능 구역 없음")));
					}
				}

				else
				{
					bIsValidArea = true;
				}
			}

			else
			{
				TryCount++;

				if (TryCount >= 50)
				{
					UE_LOG(LogTemp, Warning, (TEXT("스폰 가능 구역 없음")));
				}

			}
		}

		else
		{
			TryCount++;

			if (TryCount >= 50)
			{
				UE_LOG(LogTemp, Warning, (TEXT("스폰 가능 구역 없음"))); 
			}
		}
	}

	FRotator RandRotation = FRotator(FMath::FRandRange(0.0f, 360.f), FMath::FRandRange(0.0f, 360.f), FMath::FRandRange(0.0f, 360.f));

	return FTransform(RandRotation, SpawnLocation);
}

void ADFItemSpawner::SpawnItemNumberLimit(int32 LimitNumber)
{
	if (LimitNumber >= 1)
	{
		while (SpawnCount < LimitNumber)
		{
			SpawnItem();
			SpawnCount++;
		}
	}

	else
	{
		return;
	}
}

int32 ADFItemSpawner::GetSpawnCount() const
{
	return SpawnCount;
}

