#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DFBaseItem.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Melee,
	Firearm,
	Survive,
	Wearable,
	Effect,
};

UCLASS()
class DFPROJECT_API UDFBaseItem : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, Category = "Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, Category = "Item")
	FText DisplayItemName;

	UPROPERTY(EditAnywhere, Category = "Item")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Item")
	USkeletalMesh* ItemMesh;

	UPROPERTY(EditAnywhere, Category = "Item")
	FName EquipSocketName;

	UPROPERTY(EditAnywhere, Category = "Item")
	FPrimaryAssetType AssetType;

	UFUNCTION(BlueprintCallable, Category = "Item")
	FText GetDisplayItemName() const;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
