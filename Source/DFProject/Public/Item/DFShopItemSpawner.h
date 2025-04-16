#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DFShopItemSpawner.generated.h"

class ADFCharacter;

UCLASS()
class DFPROJECT_API ADFShopItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ADFShopItemSpawner();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ShopItemSpawner")
	USceneComponent* Scene;

	TArray<AActor*> SpawnPoints;


	UFUNCTION(BlueprintCallable, Category = "ShopItemSpawner")
	void SpawnItemToShop();
	UFUNCTION(BlueprintCallable, Category = "ShopItemSpawner")
	FTransform SetSpawnTransform(int32 LocationIndex);
	UFUNCTION(BlueprintCallable, Category = "ShopItemSpawner")
	void EquipItemToCharacter();

protected:
	virtual void BeginPlay() override;
	ADFCharacter* PlayerCharacter;
};
