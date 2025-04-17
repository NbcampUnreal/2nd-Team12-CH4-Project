#pragma once

#include "CoreMinimal.h"
#include "Item/DFItemBaseActor.h"
#include "DFWearableItemActor.generated.h"

class UDFWearableItem;

UCLASS()
class DFPROJECT_API ADFWearableItemActor : public ADFItemBaseActor
{
	GENERATED_BODY()

public:
	ADFWearableItemActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | ShopItemSpawner")
	USceneComponent* Scene;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | ShopItemSpawner")
	USkeletalMeshComponent* PreviewMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | ShopItemSpawner")
	UDFWearableItem* WearableItemData;

	UFUNCTION(BlueprintCallable, Category = "Item | ShopItemSpawner")
	void SetActor(UDFWearableItem* ItemDataToShow);
};
