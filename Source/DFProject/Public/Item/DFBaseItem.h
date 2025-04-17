#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DFBaseItem.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	OneHand,
	TwoHAnd,
	Firearm,
	Survive,
	Eyes,
	Head,
	Fabric,
	Effect,
	Special,
};

class ADFItemBaseActor;
class UAbilityStrategy;

UCLASS()
class DFPROJECT_API UDFBaseItem : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:	
	UDFBaseItem();

	UPROPERTY(EditAnywhere, Category = "Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, Category = "Item")
	FText DisplayItemName;

	UPROPERTY(EditAnywhere, Category = "Item")
	int32 ItemPrice;

	UPROPERTY(EditAnywhere, Category = "Item")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Item")
	USkeletalMesh* ItemMesh;

	UPROPERTY(EditAnywhere, Category = "Item")
	FName EquipSocketName;

	UPROPERTY(EditAnywhere, Category = "Item")
	FPrimaryAssetType AssetType;

	UPROPERTY(EditAnywhere, Category = "Item")
	TSubclassOf<UAnimInstance> AnimBP;

	UFUNCTION(BlueprintCallable, Category = "Item")
	FText GetDisplayItemName() const;

	UFUNCTION(BlueprintCallable, Category = "Item")
	FName GetItemId() const;

	UFUNCTION(BlueprintCallable, Category = "Item")
	int32 GetItemPrice() const;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ADFItemBaseActor> ItemActorClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAbilityStrategy> CharacterAbility;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	
};
